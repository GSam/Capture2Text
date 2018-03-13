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

#ifndef POPUPDIALOG_H
#define POPUPDIALOG_H

#include <QDialog>
#include "Translate.h"

namespace Ui {
class PopupDialog;
}

class PopupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PopupDialog(QWidget *parent = 0);
    ~PopupDialog();
    void setText(QString ocrText, QString translation);
    QString getOcrText();
    void setTopmost(bool topmost);

private slots:
    void on_PopupDialog_accepted();
    void on_PopupDialog_finished(int result);
    void on_checkBoxTopmost_toggled(bool checked);

    void on_labelFont_linkActivated(const QString &link);

    void on_pushButtonTranslate_clicked();

    void on_pushButtonSpeech_clicked();

protected:
    void showEvent(QShowEvent *event);

private:
    void translationComplete(QString phrase, QString translation, bool error);

    Ui::PopupDialog *ui;
    bool settingTopmost;
    Translate translate;
};

#endif // POPUPDIALOG_H
