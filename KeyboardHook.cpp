/*
Copyright (C) 2010-2017 Christopher Brochtrup

This file is part of Capture2Text.

Capture2Text is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Capture2Text is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Capture2Text.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "KeyboardHook.h"

void KeyboardHook::run()
{
    if(hHook == nullptr)
    {
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, nullptr, 0);

        if (hHook == nullptr)
        {
            qDebug() << "Keyboard Hook Failed!";
            return;
        }
    }

    QEventLoop eventLoop;
    eventLoop.exec();
}

void KeyboardHook::addHotkey(int id, Hotkey hotkey)
{
    if(hotkeys.contains(id))
    {
        if(hotkey.getVkCode() == 0)
        {
            hotkeys.remove(id);
        }
        else
        {
            hotkeys[id] = hotkey;
        }
    }
    else
    {
        hotkeys.insert(id, hotkey);
    }
}

void KeyboardHook::removeHotkey(int id)
{
    if(hotkeys.contains(id))
    {
        hotkeys.remove(id);
    }
}

void KeyboardHook::endThread()
{
    if(hHook != nullptr)
    {
        UnhookWindowsHookEx(hHook);
        hHook = nullptr;
    }

    exit(0);
}

LRESULT CALLBACK KeyboardHook::hookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode < 0)
    {
        return CallNextHookEx(KeyboardHook::getInstance().getHHook(), nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT kbData = *((KBDLLHOOKSTRUCT*)lParam);

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
    {
        bool modCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool modShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool modAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool modWin = (GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0;

        //qDebug() << "Key Pressed: " << kbData.vkCode;

        for(auto id : KeyboardHook::getInstance().getHotkeys().keys())
        {
            Hotkey key = KeyboardHook::getInstance().getHotkeys()[id];

            if(key.getVkCode() == kbData.vkCode
                    && (key.getModCtrl() == modCtrl)
                    && (key.getModShift() == modShift)
                    && (key.getModAlt() == modAlt)
                    && (key.getModWin() == modWin))
            {
                // qDebug() << "Hotkey ID: " << id;
                emit KeyboardHook::getInstance().keyPressed(id);

                // Suppress Start Menu and Alt Menu by sending a Ctrl up/down keypress.
                // So WinKey becomes WinKey+Ctrl and Alt becomes Alt+Ctrl.
                if(!modCtrl && !modShift
                        && ((modWin && !modAlt) || (modAlt && !modWin)))
                {
                    int numInputs = 1;
                    INPUT input;
                    memset(&input, 0, sizeof(INPUT));
                    input.type = INPUT_KEYBOARD;
                    input.ki.wVk = VK_CONTROL;
                    SendInput(numInputs, &input, sizeof(INPUT)); // Ctrl down

                    input.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(numInputs, &input, sizeof(INPUT)); // Ctrl up
                }

                return 1;
            }
        }
    }

    return CallNextHookEx(KeyboardHook::getInstance().getHHook(), nCode, wParam, lParam);
}
