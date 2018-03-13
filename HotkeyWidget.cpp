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

#include "HotkeyWidget.h"
#include "ui_HotkeyWidget.h"
#include "Hotkey.h"

HotkeyWidget::HotkeyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HotkeyWidget)
{
    ui->setupUi(this);

    QList<KeyNameCode> keyNameCode = Hotkey::getKeyNameCodes();

    for(auto item : keyNameCode)
    {
        // Don't add reserved hotkeys to list
        if(item.name != "Enter"
                && item.name != "Esc"
                && item.name != "Left"
                && item.name != "Right"
                && item.name != "Up"
                && item.name != "Down"
                )
        {
          ui->comboBoxKeys->addItem(item.name);
        }
    }
}

HotkeyWidget::~HotkeyWidget()
{
    delete ui;
}

void HotkeyWidget::setHotkey(Hotkey hotkey)
{
    ui->checkBoxCtrl->setChecked(hotkey.getModCtrl());
    ui->checkBoxShift->setChecked(hotkey.getModShift());
    ui->checkBoxAlt->setChecked(hotkey.getModAlt());
    ui->checkBoxWin->setChecked(hotkey.getModWin());
    ui->comboBoxKeys->setCurrentText(hotkey.getVkCodeName());
}

Hotkey HotkeyWidget::getHotkey()
{
    return Hotkey(ui->checkBoxCtrl->isChecked(),
                  ui->checkBoxShift->isChecked(),
                  ui->checkBoxAlt->isChecked(),
                  ui->checkBoxWin->isChecked(),
                  Hotkey::keyNameToVkCode(ui->comboBoxKeys->currentText()));
}
