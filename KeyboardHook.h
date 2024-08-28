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

#ifndef KEYBOARD_HOOK_H
#define KEYBOARD_HOOK_H

#include <QThread>
#include <QEventLoop>
#include <QDebug>
#include <QMap>
#ifdef __WIN32
#include "Windows.h"
#endif
#include "Hotkey.h"

class KeyboardHook : public QThread
{
    Q_OBJECT
public:
    void run();

    static KeyboardHook& getInstance()
    {
        static KeyboardHook instance;
        return instance;
    }
#ifdef __WIN32
    HHOOK getHHook() const { return hHook; }
#endif
    QMap<int, Hotkey> getHotkeys() { return hotkeys; }

    void addHotkey(int id, Hotkey hotkey);
    void removeHotkey(int id);

    void endThread();

signals:
    void keyPressed(int id);

private:
    #ifdef __WIN32
    HHOOK hHook;
#endif
    QMap<int, Hotkey> hotkeys;

    KeyboardHook()
        #ifdef __WIN32
        : hHook(nullptr)
#endif
    {
        start();
    }
#ifdef __WIN32
    static LRESULT CALLBACK hookProc(int nCode, WPARAM wParam, LPARAM lParam);
#endif
};

#endif // KEYBOARD_HOOK_H
