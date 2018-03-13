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


#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QFileDialog>
#include <QTextToSpeech>

#include "OcrEngine.h"
#include "Settings.h"
#include "SettingsDialog.h"
#include "Translate.h"
#include "ui_SettingsDialog.h"
#include "UtilsCommon.h"
#include "Speech.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    setWindowFlags(windowFlags()
                   & ~Qt::WindowContextHelpButtonHint
                   | Qt::WindowMinMaxButtonsHint);
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::init()
{
    ui->samplePreviewBox->setIsCaptureBoxSample(false);

    ui->comboBoxOcrCurrentLang->addItems(OcrEngine::getInstalledLangs());
    ui->comboBoxOcrQuickAccess1->addItems(OcrEngine::getInstalledLangs());
    ui->comboBoxOcrQuickAccess2->addItems(OcrEngine::getInstalledLangs());
    ui->comboBoxOcrQuickAccess3->addItems(OcrEngine::getInstalledLangs());

    ui->comboBoxOcrTextOrientation->addItems(QStringList() << "Auto" << "Horizontal" << "Vertical");

    ui->comboBoxPreviewBoxPosition->addItem(QIcon(":/img/img/Preview_Fixed_TopLeft.png"), "Fixed - Top Left");
    ui->comboBoxPreviewBoxPosition->addItem(QIcon(":/img/img/Preview_Fixed_BottomLeft.png"), "Fixed - Bottom Left");
    ui->comboBoxPreviewBoxPosition->addItem(QIcon(":/img/img/Preview_Dynamic_TopLeft.png"), "Dynamic - Top Left");
    ui->comboBoxPreviewBoxPosition->addItem(QIcon(":/img/img/Preview_Dynamic_BottomLeft.png"), "Dynamic - Bottom Left");
    ui->comboBoxPreviewBoxPosition->addItem(QIcon(":/img/img/Preview_Dynamic_RightEdgeTop.png"), "Dynamic - Right Edge Top");
    ui->comboBoxPreviewBoxPosition->addItem(QIcon(":/img/img/Preview_Dynamic_RightEdgeCenter.png"), "Dynamic - Right Edge Center");
    ui->comboBoxPreviewBoxPosition->addItem(QIcon(":/img/img/Preview_Dynamic_RightEdgeBottom.png"), "Dynamic - Right Edge Bottom");

    ui->comboBoxReplaceLang->addItems(OcrEngine::getInstalledLangs());

    ui->comboBoxTranslateSeparator->addItems(QStringList() << "Space" << "Tab" << "Line Break" << "Comma" << "Semicolon" << "Pipe");

    QTextToSpeech speech;
    QVector<QLocale> locales = speech.availableLocales();

    foreach (const QLocale &locale, locales)
    {
        speech.setLocale(locale);

        auto voiceList = speech.availableVoices();

        foreach (const QVoice &voice, voiceList)
        {
            QStringList tag = QStringList() << locale.name() << voice.name();
            QString displayName(QString("%1 (%2) - %3")
                                .arg(QLocale::languageToString(locale.language()))
                                .arg(QLocale::countryToString(locale.country()))
                                .arg(voice.name()));

            ui->comboBoxSpeechVoice->addItem(displayName, tag);
        }
    }

    ui->comboBoxSpeechVoice->addItem("<Disabled>", QStringList() << "<Disabled>" << "<Disabled>");

    ui->comboBoxSpeechLang->addItems(OcrEngine::getInstalledLangs());
}

void SettingsDialog::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    ui->widgetHotkeyCaptureBox->setHotkey(Hotkey(Settings::getHotkeyCaptureBox()));
    ui->widgetHotkeyRecaptureLast->setHotkey(Hotkey(Settings::getHotkeyReCaptureLast()));
    ui->widgetHotkeyTextLineCapture->setHotkey(Hotkey(Settings::getHotkeyTextLineCapture()));
    ui->widgetHotkeyForwardTextLineCapture->setHotkey(Hotkey(Settings::getHotkeyForwardTextLineCapture()));
    ui->widgetHotkeyBubbleCapture->setHotkey(Hotkey(Settings::getHotkeyBubbleCapture()));
    ui->widgetHotkeyLang1->setHotkey(Hotkey(Settings::getHotkeyLang1()));
    ui->widgetHotkeyLang2->setHotkey(Hotkey(Settings::getHotkeyLang2()));
    ui->widgetHotkeyLang3->setHotkey(Hotkey(Settings::getHotkeyLang3()));
    ui->widgetHotkeyTextOrientation->setHotkey(Hotkey(Settings::getHotkeyTextOrientation()));
    ui->widgetHotkeyWhitelist->setHotkey(Hotkey(Settings::getHotkeyWhitelist()));
    ui->widgetHotkeyBlacklist->setHotkey(Hotkey(Settings::getHotkeyBlacklist()));

    ui->comboBoxOcrCurrentLang->setCurrentText(Settings::getOcrLang());
    ui->comboBoxOcrQuickAccess1->setCurrentText(Settings::getOcrQuickAccessLang1());
    ui->comboBoxOcrQuickAccess2->setCurrentText(Settings::getOcrQuickAccessLang2());
    ui->comboBoxOcrQuickAccess3->setCurrentText(Settings::getOcrQuickAccessLang3());

    ui->checkBoxOcrWhitelistEnable->setChecked(Settings::getOcrEnableWhitelist());
    ui->lineEditOcrWhiteList->setText(Settings::getOcrWhitelist());
    ui->checkBoxOcrBlacklistEnable->setChecked(Settings::getOcrEnableBlacklist());
    ui->lineEditOcrBlackList->setText(Settings::getOcrBlacklist());
    ui->comboBoxOcrTextOrientation->setCurrentText(Settings::getOcrTextOrientation());
    ui->lineEditOcrTesseractConfigFile->setText(Settings::getOcrTesseractConfigFile());
    ui->doubleSpinBoxOcrScaleFactor->setValue(Settings::getOcrScaleFactor());
    ui->checkBoxPreprocessTrim->setChecked(Settings::getOcrTrim());
    ui->checkBoxDeskew->setChecked(Settings::getOcrDeskew());

    ui->spinBoxTextLineCaptureLength->setValue(Settings::getTextLineCaptureLength());
    ui->spinBoxTextLineCaptureWidth->setValue(Settings::getTextLineCaptureWidth());
    ui->spinBoxTextLineCaptureLookahead->setValue(Settings::getTextLineCaptureLookahead());
    ui->spinBoxTextLineCaptureLookbehind->setValue(Settings::getTextLineCaptureLookbehind());
    ui->spinBoxTextLineCaptureSearchRadius->setValue(Settings::getTextLineCaptureSearchRadius());

    ui->spinBoxForwardTextLineCaptureLength->setValue(Settings::getForwardTextLineCaptureLength());
    ui->spinBoxForwardTextLineCaptureWidth->setValue(Settings::getForwardTextLineCaptureWidth());
    ui->spinBoxForwardTextLineCaptureLookahead->setValue(Settings::getForwardTextLineCaptureLookahead());
    ui->spinBoxForwardTextLineCaptureLookbehind->setValue(Settings::getForwardTextLineCaptureLookbehind());
    ui->spinBoxForwardTextLineCaptureStartOffset->setValue(Settings::getForwardTextLineCaptureStartOffset());
    ui->spinBoxForwardTextLineCaptureSearchRadius->setValue(Settings::getForwardTextLineCaptureSearchRadius());
    ui->checkBoxForwardTextLineCaptureFirstWord->setChecked(Settings::getForwardTextLineCaptureFirstWord());

    ui->spinBoxBubbleCaptureWidth->setValue(Settings::getBubbleCaptureWidth());
    ui->spinBoxBubbleCaptureHeight->setValue(Settings::getBubbleCaptureHeight());

    ui->sampleCaptureBox->setBackgroundColor(Settings::getCaptureBoxBackgroundColor());
    ui->sampleCaptureBox->setBorderColor(Settings::getCaptureBoxBorderColor());

    ui->checkBoxPreviewBoxEnable->setChecked(Settings::getPreviewEnabled());
    ui->comboBoxPreviewBoxPosition->setCurrentText(Settings::getPreviewPosition());
    ui->samplePreviewBox->setBackgroundColor(Settings::getPreviewBackgroundColor());
    ui->samplePreviewBox->setBorderColor(Settings::getPreviewBorderColor());
    ui->samplePreviewBox->setTextColor(Settings::getPreviewTextColor());
    ui->samplePreviewBox->setTextFont(Settings::getPreviewTextFont());

    ui->checkBoxOutputClipboard->setChecked(Settings::getOutputClipboard());
    ui->checkBoxOutputShowPopup->setChecked(Settings::getOutputShowPopup());
    ui->checkBoxOutputKeepLineBreaks->setChecked(Settings::getOutputKeepLineBreaks());
    ui->checkBoxOutputLoggingEnable->setChecked(Settings::getOutputLogFileEnable());
    ui->lineEditOutputLoggingFile->setText(Settings::getOutputLogFile());
    ui->lineEditOutputLoggingFormat->setText(Settings::getOutputLogFormat());

    ui->checkBoxDebugSaveCapturedImage->setChecked(Settings::getDebugSaveCaptureImage());
    ui->checkBoxDebugSaveEnhancedImage->setChecked(Settings::getDebugSaveEnhancedImage());
    ui->checkBoxDebugAppendTimestampToImage->setChecked(Settings::getDebugAppendTimestampToImage());
    ui->checkBoxDebugPrependCoords->setChecked(Settings::getDebugPrependCoords());

    ui->comboBoxReplaceLang->setCurrentText(Settings::getOcrLang());
    setReplacementTable(Settings::getOcrLang());

    ui->checkBoxTranslateClipboard->setChecked(Settings::getTranslateAddToClipboard());
    ui->checkBoxTranslatePopup->setChecked(Settings::getTranslateAddToPopup());
    ui->comboBoxTranslateSeparator->setCurrentText(Settings::getTranslateSeparator());
    ui->spinBoxTranslateTimeout->setValue(Settings::getTranslateServerTimeout());
    populateTranslateTable();

    ui->comboBoxSpeechLang->setCurrentText(Settings::getOcrLang());
    ui->checkBoxSpeechEnable->setChecked(Settings::getSpeechEnable());
    ui->horizontalSliderSpeechVolume->setValue(Settings::getSpeechVolume());
    setSpeechInfo(Settings::getOcrLang());
}


void SettingsDialog::setReplacementTable(QString lang)
{
    lastReplacementLang = lang;
    QList<Replacement> replacementList = Settings::getOcrReplacementList(lang);
    ui->tableWidgetReplace->clearContents();
    ui->tableWidgetReplace->setRowCount(replacementList.size() + 20);
    ui->tableWidgetReplace->setColumnCount(2);
    ui->tableWidgetReplace->setHorizontalHeaderLabels(QStringList() << "Find (regex)" << "Replace With");

    int row = 0;

    for(auto item : replacementList)
    {
        ui->tableWidgetReplace->setItem(row, 0, new QTableWidgetItem(item.from));
        ui->tableWidgetReplace->setItem(row, 1, new QTableWidgetItem(item.to));
        row++;
    }
}

void SettingsDialog::saveReplacementTable(QString lang)
{
    QList<Replacement> list;

    for(int rowIdx = 0; rowIdx < ui->tableWidgetReplace->rowCount(); rowIdx++)
    {
        QTableWidgetItem* fromItem = ui->tableWidgetReplace->item(rowIdx, 0);

        if(fromItem != nullptr && fromItem->text().length() > 0)
        {
            QTableWidgetItem* toItem = ui->tableWidgetReplace->item(rowIdx, 1);

            QString toStr = "";

            if(toItem != nullptr)
            {
                toStr = toItem->text();
            }

            list.append(Replacement(fromItem->text(), toStr));
        }
    }

    Settings::setOcrReplacementList(lang, list);
}

void SettingsDialog::saveTranslateTable()
{
    for(int rowIdx = 0; rowIdx < ui->tableWidgetTranslate->rowCount(); rowIdx++)
    {
        QTableWidgetItem* ocrLangItem = ui->tableWidgetTranslate->item(rowIdx, 0);

        if(ocrLangItem != nullptr && ocrLangItem->text().length() > 0)
        {
            QComboBox* translateItem = (QComboBox *)ui->tableWidgetTranslate->cellWidget(rowIdx, 1);

            if(translateItem != nullptr)
            {
                Settings::setTranslateLang(ocrLangItem->text(), translateItem->currentText());
            }
        }
    }
}

void SettingsDialog::populateTranslateTable()
{
    QStringList ocrLangs = OcrEngine::getInstalledLangs();
    ui->tableWidgetTranslate->clearContents();
    ui->tableWidgetTranslate->setRowCount(ocrLangs.length());
    ui->tableWidgetTranslate->setColumnCount(2);
    ui->tableWidgetTranslate->setHorizontalHeaderLabels(QStringList() << "OCR Language" << "Translate To (using Google Translate)");

    int row = 0;

    QStringList translateLangs = Translate::getAvailableLangs();

    for(auto ocrLang : ocrLangs)
    {
        if(Translate::isValidLang(OcrEngine::altLangToLang(ocrLang)))
        {
            ui->tableWidgetTranslate->setItem(row, 0, new QTableWidgetItem(ocrLang));

            QComboBox* langDropDown = new QComboBox();
            langDropDown->addItem("<Do Not Translate>");
            langDropDown->addItems(getTranslateLangsSansOcrLang(ocrLang, translateLangs));
            langDropDown->setCurrentText(Settings::getTranslateLang(ocrLang));
            ui->tableWidgetTranslate->setCellWidget(row, 1, langDropDown);

            row++;
        }
    }
}

QStringList SettingsDialog::getTranslateLangsSansOcrLang(QString ocrLang, QStringList translateLangs)
{
    QStringList newList;

    for(auto lang : translateLangs)
    {
        if(lang != OcrEngine::altLangToLang(ocrLang))
        {
            newList.append(lang);
        }
    }

    return newList;
}

void SettingsDialog::on_SettingsDialog_accepted()
{
    Settings::setHotkeyCaptureBox(ui->widgetHotkeyCaptureBox->getHotkey().toStr());
    Settings::setHotkeyReCaptureLast(ui->widgetHotkeyRecaptureLast->getHotkey().toStr());
    Settings::setHotkeyTextLineCapture(ui->widgetHotkeyTextLineCapture->getHotkey().toStr());
    Settings::setHotkeyForwardTextLineCapture(ui->widgetHotkeyForwardTextLineCapture->getHotkey().toStr());
    Settings::setHotkeyBubbleCapture(ui->widgetHotkeyBubbleCapture->getHotkey().toStr());
    Settings::setHotkeyLang1(ui->widgetHotkeyLang1->getHotkey().toStr());
    Settings::setHotkeyLang2(ui->widgetHotkeyLang2->getHotkey().toStr());
    Settings::setHotkeyLang3(ui->widgetHotkeyLang3->getHotkey().toStr());
    Settings::setHotkeyTextOrientation(ui->widgetHotkeyTextOrientation->getHotkey().toStr());
    Settings::setHotkeyWhitelist(ui->widgetHotkeyWhitelist->getHotkey().toStr());
    Settings::setHotkeyBlacklist(ui->widgetHotkeyBlacklist->getHotkey().toStr());

    Settings::setOcrLang(ui->comboBoxOcrCurrentLang->currentText());
    Settings::setOcrQuickAccessLang1(ui->comboBoxOcrQuickAccess1->currentText());
    Settings::setOcrQuickAccessLang2(ui->comboBoxOcrQuickAccess2->currentText());
    Settings::setOcrQuickAccessLang3(ui->comboBoxOcrQuickAccess3->currentText());

    Settings::setOcrEnableWhitelist(ui->checkBoxOcrWhitelistEnable->isChecked());
    Settings::setOcrWhitelist(ui->lineEditOcrWhiteList->text());
    Settings::setOcrEnableBlacklist(ui->checkBoxOcrBlacklistEnable->isChecked());
    Settings::setOcrBlacklist(ui->lineEditOcrBlackList->text());
    Settings::setOcrTextOrientation(ui->comboBoxOcrTextOrientation->currentText());
    Settings::setOcrTesseractConfigFile(ui->lineEditOcrTesseractConfigFile->text());
    Settings::setOcrScaleFactor(ui->doubleSpinBoxOcrScaleFactor->value());
    Settings::setOcrTrim(ui->checkBoxPreprocessTrim->isChecked());
    Settings::setOcrDeskew(ui->checkBoxDeskew->isChecked());

    Settings::setTextLineCaptureLength(ui->spinBoxTextLineCaptureLength->value());
    Settings::setTextLineCaptureWidth(ui->spinBoxTextLineCaptureWidth->value());
    Settings::setTextLineCaptureLookahead(ui->spinBoxTextLineCaptureLookahead->value());
    Settings::setTextLineCaptureLookbehind(ui->spinBoxTextLineCaptureLookbehind->value());
    Settings::setTextLineCaptureSearchRadius(ui->spinBoxTextLineCaptureSearchRadius->value());

    Settings::setForwardTextLineCaptureLength(ui->spinBoxForwardTextLineCaptureLength->value());
    Settings::setForwardTextLineCaptureWidth(ui->spinBoxForwardTextLineCaptureWidth->value());
    Settings::setForwardTextLineCaptureLookahead(ui->spinBoxForwardTextLineCaptureLookahead->value());
    Settings::setForwardTextLineCaptureLookbehind(ui->spinBoxForwardTextLineCaptureLookbehind->value());
    Settings::setForwardTextLineCaptureStartOffset(ui->spinBoxForwardTextLineCaptureStartOffset->value());
    Settings::setForwardTextLineCaptureSearchRadius(ui->spinBoxForwardTextLineCaptureSearchRadius->value());
    Settings::setForwardTextLineCaptureFirstWord(ui->checkBoxForwardTextLineCaptureFirstWord->isChecked());

    Settings::setBubbleCaptureWidth(ui->spinBoxBubbleCaptureWidth->value());
    Settings::setBubbleCaptureHeight(ui->spinBoxBubbleCaptureHeight->value());

    Settings::setCaptureBoxBackgroundColor(ui->sampleCaptureBox->getBackgroundColor());
    Settings::setCaptureBoxBorderColor(ui->sampleCaptureBox->getBorderColor());

    Settings::setPreviewEnabled(ui->checkBoxPreviewBoxEnable->isChecked());
    Settings::setPreviewPosition(ui->comboBoxPreviewBoxPosition->currentText());
    Settings::setPreviewBackgroundColor(ui->samplePreviewBox->getBackgroundColor());
    Settings::setPreviewBorderColor(ui->samplePreviewBox->getBorderColor());
    Settings::setPreviewTextColor(ui->samplePreviewBox->getTextColor());
    Settings::setPreviewTextFont(ui->samplePreviewBox->getTextFont());

    Settings::setOutputClipboard(ui->checkBoxOutputClipboard->isChecked());
    Settings::setOutputShowPopup(ui->checkBoxOutputShowPopup->isChecked());
    Settings::setOutputKeepLineBreaks(ui->checkBoxOutputKeepLineBreaks->isChecked());
    Settings::setOutputLogFileEnable(ui->checkBoxOutputLoggingEnable->isChecked());
    Settings::setOutputLogFile(ui->lineEditOutputLoggingFile->text());
    Settings::setOutputLogFormat(ui->lineEditOutputLoggingFormat->text());

    Settings::setDebugSaveCaptureImage(ui->checkBoxDebugSaveCapturedImage->isChecked());
    Settings::setDebugSaveEnhancedImage(ui->checkBoxDebugSaveEnhancedImage->isChecked());
    Settings::setDebugAppendTimestampToImage(ui->checkBoxDebugAppendTimestampToImage->isChecked());
    Settings::setDebugPrependCoords(ui->checkBoxDebugPrependCoords->isChecked());

    saveReplacementTable(lastReplacementLang);

    Settings::setTranslateAddToClipboard(ui->checkBoxTranslateClipboard->isChecked());
    Settings::setTranslateAddToPopup(ui->checkBoxTranslatePopup->isChecked());
    Settings::setTranslateSeparator(ui->comboBoxTranslateSeparator->currentText());
    Settings::setTranslateServerTimeout(ui->spinBoxTranslateTimeout->value());

    saveTranslateTable();

    Settings::setSpeechEnable(ui->checkBoxSpeechEnable->isChecked());
    Settings::setSpeechVolume(ui->horizontalSliderSpeechVolume->value());
    saveSpeechInfo(lastSpeechLang);
}

void SettingsDialog::on_pushButtonCaptureBoxBackgroundColor_clicked()
{
    QColor color = UtilsCommon::pickColor(ui->sampleCaptureBox->getBackgroundColor());
    ui->sampleCaptureBox->setBackgroundColor(color);
    ui->sampleCaptureBox->update();
}

void SettingsDialog::on_pushButtonCaptureBoxBorderColor_clicked()
{
    QColor color = UtilsCommon::pickColor(ui->sampleCaptureBox->getBorderColor());
    ui->sampleCaptureBox->setBorderColor(color);
    ui->sampleCaptureBox->update();
}

void SettingsDialog::on_labelCaptureBoxReset_linkActivated(const QString &)
{
    ui->sampleCaptureBox->setBackgroundColor(Settings::defaultCaptureBoxBackgroundColor);
    ui->sampleCaptureBox->setBorderColor(Settings::defaultCaptureBoxBorderColor);
    ui->sampleCaptureBox->update();
}

void SettingsDialog::on_pushButtonPreviewBoxBackgroundColor_clicked()
{
    QColor color = UtilsCommon::pickColor(ui->samplePreviewBox->getBackgroundColor());
    ui->samplePreviewBox->setBackgroundColor(color);
    ui->samplePreviewBox->update();
}

void SettingsDialog::on_pushButtonPreviewBoxBorderColor_clicked()
{
    QColor color = UtilsCommon::pickColor(ui->samplePreviewBox->getBorderColor());
    ui->samplePreviewBox->setBorderColor(color);
    ui->samplePreviewBox->update();
}

void SettingsDialog::on_pushButtonPreviewBoxTextColor_clicked()
{
    QColor color = UtilsCommon::pickColor(ui->samplePreviewBox->getTextColor());
    ui->samplePreviewBox->setTextColor(color);
    ui->samplePreviewBox->update();
}

void SettingsDialog::on_pushButtonPreviewBoxFont_clicked()
{
    QFont font = UtilsCommon::pickFont(ui->samplePreviewBox->getTextFont());
    ui->samplePreviewBox->setTextFont(font);
    ui->samplePreviewBox->update();
}

void SettingsDialog::on_labelPreviewBoxReset_linkActivated(const QString &)
{
    ui->checkBoxPreviewBoxEnable->setChecked(Settings::defaultPreviewEnabled);
    ui->comboBoxPreviewBoxPosition->setCurrentText(Settings::defaultPreviewPosition);
    ui->samplePreviewBox->setBackgroundColor(Settings::defaultPreviewBackgroundColor);
    ui->samplePreviewBox->setBorderColor(Settings::defaultPreviewBorderColor);
    ui->samplePreviewBox->setTextColor(Settings::defaultPreviewTextColor);
    ui->samplePreviewBox->setTextFont(Settings::defaultPreviewTextFont);
    ui->samplePreviewBox->update();
}

void SettingsDialog::on_labelHotkeysReset_linkActivated(const QString &)
{
    ui->widgetHotkeyCaptureBox->setHotkey(Hotkey(Settings::defaultHotkeyCaptureBox));
    ui->widgetHotkeyRecaptureLast->setHotkey(Hotkey(Settings::defaultHotkeyReCaptureLast));
    ui->widgetHotkeyTextLineCapture->setHotkey(Hotkey(Settings::defaultHotkeyTextLineCapture));
    ui->widgetHotkeyForwardTextLineCapture->setHotkey(Hotkey(Settings::defaultHotkeyForwardTextLineCapture));
    ui->widgetHotkeyBubbleCapture->setHotkey(Hotkey(Settings::defaultHotkeyBubbleCapture));
    ui->widgetHotkeyLang1->setHotkey(Hotkey(Settings::defaultHotkeyLang1));
    ui->widgetHotkeyLang2->setHotkey(Hotkey(Settings::defaultHotkeyLang2));
    ui->widgetHotkeyLang3->setHotkey(Hotkey(Settings::defaultHotkeyLang3));
    ui->widgetHotkeyTextOrientation->setHotkey(Hotkey(Settings::defaultHotkeyTextOrientation));
    ui->widgetHotkeyWhitelist->setHotkey(Hotkey(Settings::defaultHotkeyWhitelist));
    ui->widgetHotkeyBlacklist->setHotkey(Hotkey(Settings::defaultHotkeyBlacklist));
}

void SettingsDialog::on_labelOcrAdditionalLang_linkActivated(const QString &)
{
    QDesktopServices::openUrl(QUrl("http://capture2text.sourceforge.net/#install_additional_languages"));
}

void SettingsDialog::on_pushButtonOcrWhiteListAddLowercase_clicked()
{
    if(!ui->lineEditOcrWhiteList->text().contains("abcdefghijklmnopqrstuvwxyz"))
    {
        ui->lineEditOcrWhiteList->insert("abcdefghijklmnopqrstuvwxyz");
    }
}

void SettingsDialog::on_pushButtonOcrWhiteListAddUppercase_clicked()
{
    if(!ui->lineEditOcrWhiteList->text().contains("ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
    {
        ui->lineEditOcrWhiteList->insert("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }
}

void SettingsDialog::on_pushButtonOcrWhiteListAddNumbers_clicked()
{
    if(!ui->lineEditOcrWhiteList->text().contains("0123456789"))
    {
        ui->lineEditOcrWhiteList->insert("0123456789");
    }
}

void SettingsDialog::on_pushButtonOcrBlackListAddLowercase_clicked()
{
    if(!ui->lineEditOcrBlackList->text().contains("abcdefghijklmnopqrstuvwxyz"))
    {
        ui->lineEditOcrBlackList->insert("abcdefghijklmnopqrstuvwxyz");
    }
}

void SettingsDialog::on_pushButtonOcrBlackListAddUppercase_clicked()
{
    if(!ui->lineEditOcrBlackList->text().contains("ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
    {
        ui->lineEditOcrBlackList->insert("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }
}

void SettingsDialog::on_pushButtonOcrBlackListAddNumbers_clicked()
{
    if(!ui->lineEditOcrBlackList->text().contains("0123456789"))
    {
        ui->lineEditOcrBlackList->insert("0123456789");
    }
}

void SettingsDialog::on_labelOcrOptions1Reset_linkActivated(const QString &)
{
    if(OcrEngine::isLangInstalled(Settings::defaultOcrLang))
    {
        ui->comboBoxOcrCurrentLang->setCurrentText(Settings::defaultOcrLang);
    }

    if(OcrEngine::isLangInstalled(Settings::defaultOcrQuickAccessLang1))
    {
        ui->comboBoxOcrQuickAccess1->setCurrentText(Settings::defaultOcrQuickAccessLang1);
    }

    if(OcrEngine::isLangInstalled(Settings::defaultOcrQuickAccessLang2))
    {
        ui->comboBoxOcrQuickAccess2->setCurrentText(Settings::defaultOcrQuickAccessLang2);
    }

    if(OcrEngine::isLangInstalled(Settings::defaultOcrQuickAccessLang3))
    {
        ui->comboBoxOcrQuickAccess3->setCurrentText(Settings::defaultOcrQuickAccessLang3);
    }

    ui->checkBoxOcrWhitelistEnable->setChecked(Settings::defaultOcrEnableWhitelist);
    ui->lineEditOcrWhiteList->setText(Settings::defaultOcrWhitelist);
    ui->checkBoxOcrBlacklistEnable->setChecked(Settings::defaultOcrEnableBlacklist);
    ui->lineEditOcrBlackList->setText(Settings::defaultOcrBlacklist);
    ui->comboBoxOcrTextOrientation->setCurrentText(Settings::defaultOcrTextOrientation);
    ui->lineEditOcrTesseractConfigFile->setText(Settings::defaultOcrTesseractConfigFile);
    ui->doubleSpinBoxOcrScaleFactor->setValue(Settings::defaultOcrScaleFactor);
    ui->checkBoxPreprocessTrim->setChecked(Settings::defaultOcrTrim);
    ui->checkBoxDeskew->setChecked(Settings::defaultOcrDeskew);
}

void SettingsDialog::on_pushButtonOcrTesseractConfigFile_clicked()
{
    QString tessConfigFile = QFileDialog::getOpenFileName(this, "Select File");
    ui->lineEditOcrTesseractConfigFile->setText(tessConfigFile);
}

void SettingsDialog::on_labelOcrOptions2Reset_linkActivated(const QString &)
{
    ui->spinBoxTextLineCaptureLength->setValue(Settings::defaultTextLineCaptureLength);
    ui->spinBoxTextLineCaptureWidth->setValue(Settings::defaultTextLineCaptureWidth);
    ui->spinBoxTextLineCaptureLookahead->setValue(Settings::defaultTextLineCaptureLookahead);
    ui->spinBoxTextLineCaptureLookbehind->setValue(Settings::defaultTextLineCaptureLookbehind);
    ui->spinBoxTextLineCaptureSearchRadius->setValue(Settings::defaultTextLineCaptureSearchRadius);

    ui->spinBoxForwardTextLineCaptureLength->setValue(Settings::defaultForwardTextLineCaptureLength);
    ui->spinBoxForwardTextLineCaptureWidth->setValue(Settings::defaultForwardTextLineCaptureWidth);
    ui->spinBoxForwardTextLineCaptureLookahead->setValue(Settings::defaultForwardTextLineCaptureLookahead);
    ui->spinBoxForwardTextLineCaptureLookbehind->setValue(Settings::defaultForwardTextLineCaptureLookbehind);
    ui->spinBoxForwardTextLineCaptureStartOffset->setValue(Settings::defaultForwardTextLineCaptureStartOffset);
    ui->spinBoxForwardTextLineCaptureSearchRadius->setValue(Settings::defaultForwardTextLineCaptureSearchRadius);
    ui->checkBoxForwardTextLineCaptureFirstWord->setChecked(Settings::defaultForwardTextLineCaptureFirstWord);

    ui->spinBoxBubbleCaptureWidth->setValue(Settings::defaultBubbleCaptureWidth);
    ui->spinBoxBubbleCaptureHeight->setValue(Settings::defaultBubbleCaptureHeight);
}

void SettingsDialog::on_pushButtonOutputLoggingFile_clicked()
{
    QString logFile = QFileDialog::getSaveFileName(this, "Select File");
    ui->lineEditOutputLoggingFile->setText(logFile);
}

void SettingsDialog::on_labelOutputReset_linkActivated(const QString &)
{
    ui->checkBoxOutputClipboard->setChecked(Settings::defaultOutputClipbaord);
    ui->checkBoxOutputShowPopup->setChecked(Settings::defaultOutputShowPopup);
    ui->checkBoxOutputKeepLineBreaks->setChecked(Settings::defaultOutputKeepLineBreaks);
    ui->checkBoxOutputLoggingEnable->setChecked(Settings::defaultOutputLogFileEnable);
    ui->lineEditOutputLoggingFile->setText(Settings::defaultOutputLogFileEnable);
    ui->lineEditOutputLoggingFormat->setText(Settings::defaultOutputLogFormat);

    ui->checkBoxDebugSaveCapturedImage->setChecked(Settings::defaultDebugSaveCaptureImage);
    ui->checkBoxDebugSaveEnhancedImage->setChecked(Settings::defaultDebugSaveEnhancedImage);
    ui->checkBoxDebugAppendTimestampToImage->setChecked(Settings::defaultDebugAppendTimestampToImage);
    ui->checkBoxDebugPrependCoords->setChecked(Settings::defaultDebugPrependCoords);
}

void SettingsDialog::on_comboBoxReplaceLang_currentTextChanged(const QString &text)
{
    saveReplacementTable(lastReplacementLang);
    setReplacementTable(text);
}

void SettingsDialog::on_pushButtonReplaceAddRows_clicked()
{
    ui->tableWidgetReplace->setRowCount(ui->tableWidgetReplace->rowCount() + 10);
}

void SettingsDialog::on_pushButtonReplaceClearSelection_clicked()
{
    QList<QTableWidgetItem *> items = ui->tableWidgetReplace->selectedItems();

    for(auto item : items)
    {
        item->setText("");
    }
}

void SettingsDialog::on_comboBoxSpeechLang_currentTextChanged(const QString &text)
{
    saveSpeechInfo(lastSpeechLang);
    setSpeechInfo(text);
}

void SettingsDialog::on_labelSpeechReset_linkActivated(const QString &)
{
    ui->checkBoxSpeechEnable->setChecked(Settings::defaultSpeechEnable);
    ui->horizontalSliderSpeechVolume->setValue(Settings::defaultSpeechVolume);
    ui->horizontalSliderSpeechRate->setValue(Settings::defaultSpeechRate);
    ui->horizontalSliderSpeechPitch->setValue(Settings::defaultSpeechPitch);
}

void SettingsDialog::saveSpeechInfo(QString lang)
{
    QString locale = ui->comboBoxSpeechVoice->currentData().toStringList().at(0);
    QString voice = ui->comboBoxSpeechVoice->currentData().toStringList().at(1);
    Settings::setSpeechInfo(lang, locale, voice,
                            ui->horizontalSliderSpeechRate->value(),
                            ui->horizontalSliderSpeechPitch->value());
}

void SettingsDialog::setSpeechInfo(QString lang)
{
    lastSpeechLang = lang;

    QString locale;
    QString voice;
    int rate;
    int pitch;

    Settings::getSpeechInfo(lang, locale, voice, rate, pitch);

    bool voiceSet = false;

    // Try to set voice based on saved data
    for(int i = 0; i < ui->comboBoxSpeechVoice->count(); i++)
    {
        QStringList itemData = ui->comboBoxSpeechVoice->itemData(i).toStringList();

        if(itemData.at(0) == locale
                && itemData.at(1) == voice)
        {
            ui->comboBoxSpeechVoice->setCurrentText(ui->comboBoxSpeechVoice->itemText(i));
            voiceSet = true;
            break;
        }
    }

    if(!voiceSet)
    {
        ui->comboBoxSpeechVoice->setCurrentText("<Disabled>");
    }

    ui->horizontalSliderSpeechRate->setValue(rate);
    ui->horizontalSliderSpeechPitch->setValue(pitch);
}

void SettingsDialog::on_pushButtonSpeechPreview_clicked()
{
    Speech::getInstance().sayText(ui->lineEditSpeechPreview->text(),
            ui->comboBoxSpeechVoice->currentData().toStringList().at(0),
            ui->comboBoxSpeechVoice->currentData().toStringList().at(1),
            ui->horizontalSliderSpeechVolume->value(),
            ui->horizontalSliderSpeechRate->value(),
            ui->horizontalSliderSpeechPitch->value()
            );
}
