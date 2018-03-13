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

#include "Hotkey.h"
#include <windows.h>

Hotkey::Hotkey()
    : modCtrl(false), modShift(false), modAlt(false), modWin(false), vkCode(0)
{

}

Hotkey::Hotkey(bool ctrl, bool shift, bool alt, bool win, unsigned int key)
    : modCtrl(ctrl), modShift(shift), modAlt(alt), modWin(win), vkCode(key)
{

}

Hotkey::Hotkey(QString saveStr)
    : modCtrl(false), modShift(false), modAlt(false), modWin(false), vkCode(0)
{
    if(saveStr.contains("Ctrl+"))
    {
        saveStr.replace("Ctrl+", "");
        modCtrl = true;
    }

    if(saveStr.contains("Shift+"))
    {
        saveStr.replace("Shift+", "");
        modShift = true;
    }

    if(saveStr.contains("Alt+"))
    {
        saveStr.replace("Alt+", "");
        modAlt = true;
    }

    if(saveStr.contains("Win+"))
    {
        saveStr.replace("Win+", "");
        modWin = true;
    }

    vkCode = keyNameToVkCode(saveStr);
}

QString Hotkey::vkCodeToKeyName(unsigned int vkCode)
{
    for(auto nameCode : keyNameCodes)
    {
        if(nameCode.vkCode == vkCode)
        {
            return nameCode.name;
        }
    }

    return "Unknown";
}

unsigned int Hotkey::keyNameToVkCode(QString name)
{
    for(auto nameCode : keyNameCodes)
    {
        if(nameCode.name == name)
        {
            return nameCode.vkCode;
        }
    }

    return 0;
}

QString Hotkey::toStr()
{
    QString keyStr;

    if(modCtrl) { keyStr += "Ctrl+"; }
    if(modShift) { keyStr += "Shift+"; }
    if(modAlt) { keyStr += "Alt+"; }
    if(modWin) { keyStr += "Win+"; }

    keyStr += vkCodeToKeyName(vkCode);

    return keyStr;
}

void Hotkey::setModWin(bool value)
{
    modWin = value;
}

void Hotkey::setModAlt(bool value)
{
    modAlt = value;
}

void Hotkey::setModShift(bool value)
{
    modShift = value;
}

void Hotkey::setModCtrl(bool value)
{
    modCtrl = value;
}

QList<KeyNameCode> Hotkey::getKeyNameCodes()
{
    return keyNameCodes;
}

bool Hotkey::getModWin() const
{
    return modWin;
}

bool Hotkey::getModAlt() const
{
    return modAlt;
}

bool Hotkey::getModShift() const
{
    return modShift;
}

bool Hotkey::getModCtrl() const
{
    return modCtrl;
}

unsigned int Hotkey::getVkCode() const
{
    return vkCode;
}

QString Hotkey::getVkCodeName() const
{
    return vkCodeToKeyName(vkCode);
}

void Hotkey::setVkCode(unsigned int value)
{
    vkCode = value;
}


// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
QList<KeyNameCode> Hotkey::keyNameCodes = QList<KeyNameCode>()
        << KeyNameCode("<Unmapped>", 0)
        << KeyNameCode("0", '0')
        << KeyNameCode("1", '1')
        << KeyNameCode("2", '2')
        << KeyNameCode("3", '3')
        << KeyNameCode("4", '4')
        << KeyNameCode("5", '5')
        << KeyNameCode("6", '6')
        << KeyNameCode("7", '7')
        << KeyNameCode("8", '8')
        << KeyNameCode("9", '9')
        << KeyNameCode("A", 'A')
        << KeyNameCode("B", 'B')
        << KeyNameCode("C", 'C')
        << KeyNameCode("D", 'D')
        << KeyNameCode("E", 'E')
        << KeyNameCode("F", 'F')
        << KeyNameCode("G", 'G')
        << KeyNameCode("H", 'H')
        << KeyNameCode("I", 'I')
        << KeyNameCode("J", 'J')
        << KeyNameCode("K", 'K')
        << KeyNameCode("L", 'L')
        << KeyNameCode("M", 'M')
        << KeyNameCode("N", 'N')
        << KeyNameCode("O", 'O')
        << KeyNameCode("P", 'P')
        << KeyNameCode("Q", 'Q')
        << KeyNameCode("R", 'R')
        << KeyNameCode("S", 'S')
        << KeyNameCode("T", 'T')
        << KeyNameCode("U", 'U')
        << KeyNameCode("V", 'V')
        << KeyNameCode("W", 'W')
        << KeyNameCode("X", 'X')
        << KeyNameCode("Y", 'Y')
        << KeyNameCode("Z", 'Z')
        << KeyNameCode("F1", VK_F1)
        << KeyNameCode("F2", VK_F2)
        << KeyNameCode("F3", VK_F3)
        << KeyNameCode("F4", VK_F4)
        << KeyNameCode("F5", VK_F5)
        << KeyNameCode("F6", VK_F6)
        << KeyNameCode("F7", VK_F7)
        << KeyNameCode("F8", VK_F8)
        << KeyNameCode("F9", VK_F9)
        << KeyNameCode("F10", VK_F10)
        << KeyNameCode("F11", VK_F11)
        << KeyNameCode("F12", VK_F12)
        << KeyNameCode("Numpad 0", VK_NUMPAD0)
        << KeyNameCode("Numpad 1", VK_NUMPAD1)
        << KeyNameCode("Numpad 2", VK_NUMPAD2)
        << KeyNameCode("Numpad 3", VK_NUMPAD3)
        << KeyNameCode("Numpad 4", VK_NUMPAD4)
        << KeyNameCode("Numpad 5", VK_NUMPAD5)
        << KeyNameCode("Numpad 6", VK_NUMPAD6)
        << KeyNameCode("Numpad 7", VK_NUMPAD7)
        << KeyNameCode("Numpad 8", VK_NUMPAD8)
        << KeyNameCode("Numpad 9", VK_NUMPAD9)
        << KeyNameCode("Num *", VK_MULTIPLY)
        << KeyNameCode("Num +", VK_ADD)
        << KeyNameCode("Num -", VK_SUBTRACT)
        << KeyNameCode("Num .", VK_DECIMAL)
        << KeyNameCode("Num /", VK_DIVIDE)
        << KeyNameCode("~", VK_OEM_3)
        << KeyNameCode("-", VK_OEM_MINUS)
        << KeyNameCode("=", VK_OEM_PLUS)
        << KeyNameCode("[", VK_OEM_4)
        << KeyNameCode("]", VK_OEM_6)
        << KeyNameCode(";", VK_OEM_1)
        << KeyNameCode("'", VK_OEM_7)
        << KeyNameCode("\\", VK_OEM_5)
        << KeyNameCode(",", VK_OEM_COMMA)
        << KeyNameCode(".", VK_OEM_PERIOD)
        << KeyNameCode("/", VK_OEM_2)
        << KeyNameCode("Backspace", VK_BACK)
        << KeyNameCode("Tab", VK_TAB)
        << KeyNameCode("Enter", VK_RETURN)
        << KeyNameCode("Esc", VK_ESCAPE)
        << KeyNameCode("Spacebar", VK_SPACE)
        << KeyNameCode("Page up", VK_PRIOR)
        << KeyNameCode("Page down", VK_NEXT)
        << KeyNameCode("End", VK_END)
        << KeyNameCode("Home", VK_HOME)
        << KeyNameCode("Left", VK_LEFT)
        << KeyNameCode("Up", VK_UP)
        << KeyNameCode("Right", VK_RIGHT)
        << KeyNameCode("Down", VK_DOWN)
        << KeyNameCode("INS", VK_INSERT)
        << KeyNameCode("DEL", VK_DELETE)
        << KeyNameCode("Print Screen", VK_SNAPSHOT)
        << KeyNameCode("Scroll Lock", VK_SCROLL)
        << KeyNameCode("Pause", VK_PAUSE)
        //<< KeyNameCode("Application", VK_APPS)
           ;






