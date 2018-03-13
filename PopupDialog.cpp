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

#include <QClipboard>
#include <QDebug>
#include <QSize>
#include <QPalette>
#include "PopupDialog.h"
#include "ui_PopupDialog.h"
#include "Settings.h"
#include "UtilsCommon.h"
#include "OcrEngine.h"
#include "Speech.h"


PopupDialog::PopupDialog(QWidget *parent) :
    QDialog(parent),
    settingTopmost(false),
    ui(new Ui::PopupDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags()
                   & ~Qt::WindowContextHelpButtonHint
                   | Qt::WindowMinMaxButtonsHint
                   | Qt::WindowStaysOnTopHint);

//    QPalette palette;
//    QColor backgroundColor(255, 255, 255, 255);
//    QColor textColor(0, 0, 0, 255);
//    palette.setColor(QPalette::Base, backgroundColor);
//    palette.setColor(QPalette::Background, backgroundColor);
//    palette.setColor(QPalette::Text,textColor);
//    setPalette(palette);

//    QColor backgroundColor(0, 0, 0, 255);
//    QColor textColor(200, 200, 200, 255);
//    QString style = QString("background-color: %1; color: %2")
//            .arg(backgroundColor.name())
//            .arg(textColor.name());
//    setStyleSheet(style);

    ui->pushButtonSpeech->hide();
    ui->pushButtonTranslate->hide();
    ui->plainTextEditTranslation->hide();

    connect(&translate, &Translate::translationComplete, this, &PopupDialog::translationComplete);
}

PopupDialog::~PopupDialog()
{
    delete ui;
}

void PopupDialog::showEvent(QShowEvent *)
{
    ui->plainTextEditOcrText->setFont(Settings::getOutputPopupFont());
    ui->plainTextEditTranslation->setFont(Settings::getOutputPopupFont());
}

void PopupDialog::setTopmost(bool topmost)
{
    settingTopmost = true;
    Settings::setOutputPopupTopmost(topmost);

    if(topmost)
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }
    else
    {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }

    ui->checkBoxTopmost->setChecked(topmost);

    settingTopmost = false;
}

void PopupDialog::setText(QString ocrText, QString translation)
{
    ui->plainTextEditOcrText->setPlainText(ocrText);

    if(translation.length() > 0)
    {
        ui->plainTextEditTranslation->setPlainText(translation);
        ui->plainTextEditTranslation->show();
        ui->pushButtonTranslate->show();
    }
    else
    {
        ui->plainTextEditTranslation->hide();
        ui->pushButtonTranslate->hide();
    }

    if(Speech::isSpeechEnabledForCurrentLang())
    {
        ui->pushButtonSpeech->show();
    }
    else
    {
        ui->pushButtonSpeech->hide();
    }
}

QString PopupDialog::getOcrText()
{
    return ui->plainTextEditOcrText->toPlainText();
}

void PopupDialog::on_PopupDialog_accepted()
{
    if(Settings::getOutputClipboard())
    {
        if(Settings::getTranslateAddToClipboard())
        {
            QApplication::clipboard()->setText(
                        getOcrText() + Settings::separatorToStr(Settings::getTranslateSeparator())
                        + ui->plainTextEditTranslation->toPlainText());
        }
        else
        {
            QApplication::clipboard()->setText(getOcrText());
        }
    }
}

void PopupDialog::on_PopupDialog_finished(int)
{
    Settings::setOutputPopupWindowSize(QSize(width(), height()));
}

void PopupDialog::on_checkBoxTopmost_toggled(bool checked)
{
    if(settingTopmost)
    {
        return;
    }

    setTopmost(checked);
    show();
}

void PopupDialog::on_labelFont_linkActivated(const QString &)
{
    QFont font = UtilsCommon::pickFont(ui->plainTextEditOcrText->font());
    ui->plainTextEditOcrText->setFont(font);
    ui->plainTextEditTranslation->setFont(font);
    Settings::setOutputPopupFont(font);
}

void PopupDialog::on_pushButtonTranslate_clicked()
{
    QString text = ui->plainTextEditOcrText->toPlainText();

    text = text.trimmed();

    if(text.length() == 0)
    {
        ui->plainTextEditTranslation->setPlainText("");
    }
    else
    {
        QString ocrLang = OcrEngine::altLangToLang(Settings::getOcrLang());
        QString translateLang = Settings::getTranslateLang(Settings::getOcrLang());

        if(ocrLang != "None" && translateLang != "<Do Not Translate>")
        {
            if(!translate.startTranslate(text, ocrLang, translateLang, Settings::getTranslateServerTimeout()))
            {
                ui->plainTextEditTranslation->setPlainText("<Error>");
            }
            else
            {
                ui->pushButtonTranslate->setEnabled(false);
            }
        }
        else
        {
            ui->plainTextEditTranslation->setPlainText("<Set translation language in settings>");
        }
    }
}

void PopupDialog::translationComplete(QString, QString translation, bool error)
{
    ui->pushButtonTranslate->setEnabled(true);

    if(error)
    {
        ui->plainTextEditTranslation->setPlainText("<Server Timeout>");
    }
    else
    {
        ui->plainTextEditTranslation->setPlainText(translation);
    }
}

void PopupDialog::on_pushButtonSpeech_clicked()
{
    QString text = ui->plainTextEditOcrText->toPlainText();

    if(ui->plainTextEditOcrText->textCursor().selectedText().length() > 0)
    {
        text = ui->plainTextEditOcrText->textCursor().selectedText();
    }

    Speech::getInstance().sayText(text);
}
