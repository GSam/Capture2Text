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

#ifndef HOTKEY_H
#define HOTKEY_H

#include <QList>
#include <QString>
#include <windows.h>

struct KeyNameCode
{
    QString name;
    unsigned int vkCode;

    KeyNameCode(QString _name, unsigned int _vkCode)
        : name(_name), vkCode(_vkCode)
    {

    }
};

class Hotkey
{
public:
    Hotkey();
    Hotkey(bool ctrl, bool shift, bool alt, bool win, unsigned int key);
    Hotkey(QString saveStr);

    unsigned int getVkCode() const;
    QString getVkCodeName() const;
    void setVkCode(unsigned int value);
    bool getModCtrl() const;
    void setModCtrl(bool value);
    bool getModShift() const;
    void setModShift(bool value);
    bool getModAlt() const;
    void setModAlt(bool value);
    bool getModWin() const;
    void setModWin(bool value);
    static QString vkCodeToKeyName(unsigned int vkCode);
    static unsigned int keyNameToVkCode(QString name);
    static QList<KeyNameCode> getKeyNameCodes();
    QString toStr();

private:
    bool modCtrl;
    bool modShift;
    bool modAlt;
    bool modWin;
    unsigned int vkCode;

    static QList <KeyNameCode> keyNameCodes;

};


#endif // HOTKEY_H
