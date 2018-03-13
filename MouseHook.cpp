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

#include "MouseHook.h"

void MouseHook::run()
{
    if(hHook == nullptr)
    {
        hHook = SetWindowsHookEx(WH_MOUSE_LL, hookProc, nullptr, 0);

        if (hHook == nullptr)
        {
            qDebug() << "Mouse Hook Failed!";
            return;
        }

        // qDebug() << "Mouse hooked";
    }

    QEventLoop eventLoop;
    eventLoop.exec();
}

void MouseHook::endThread()
{
    if(hHook != nullptr)
    {
        UnhookWindowsHookEx(hHook);
        hHook = nullptr;
        // qDebug() << "Mouse Unhooked";
    }

    exit(0);
}

void MouseHook::setRightMouseButtonHeld(bool held)
{
    rightMouseButtonHeld = held;
}

bool MouseHook::getRightMouseButtonHeld()
{
    return rightMouseButtonHeld;
}

LRESULT CALLBACK MouseHook::hookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode < 0)
    {
        return CallNextHookEx(MouseHook::getInstance().getHHook(), nCode, wParam, lParam);
    }

    if(wParam == WM_LBUTTONDOWN)
    {
        // qDebug() << "Left mouse down";
        emit MouseHook::getInstance().buttonPressed(MouseHook::LEFT_MOUSE_DOWN);
        return 1;
    }
    else if(wParam == WM_RBUTTONDOWN)
    {
        // qDebug() << "Right mouse down";
        MouseHook::getInstance().setRightMouseButtonHeld(true);
        emit MouseHook::getInstance().buttonPressed(MouseHook::RIGHT_MOUSE_DOWN);
        return 1;
    }
    else if(wParam == WM_RBUTTONUP)
    {
        // qDebug() << "Right mouse up";
        MouseHook::getInstance().setRightMouseButtonHeld(false);
        return 1;
    }

    return CallNextHookEx(MouseHook::getInstance().getHHook(), nCode, wParam, lParam);
}
