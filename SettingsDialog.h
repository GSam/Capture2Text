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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void init();

private slots:
    void on_pushButtonCaptureBoxBackgroundColor_clicked();
    void on_SettingsDialog_accepted();
    void on_pushButtonCaptureBoxBorderColor_clicked();
    void on_labelCaptureBoxReset_linkActivated(const QString &link);
    void on_pushButtonPreviewBoxBackgroundColor_clicked();
    void on_pushButtonPreviewBoxBorderColor_clicked();
    void on_pushButtonPreviewBoxTextColor_clicked();
    void on_pushButtonPreviewBoxFont_clicked();
    void on_labelPreviewBoxReset_linkActivated(const QString &link);
    void on_labelHotkeysReset_linkActivated(const QString &link);
    void on_labelOcrAdditionalLang_linkActivated(const QString &link);
    void on_pushButtonOcrWhiteListAddLowercase_clicked();
    void on_pushButtonOcrWhiteListAddUppercase_clicked();
    void on_pushButtonOcrWhiteListAddNumbers_clicked();
    void on_pushButtonOcrBlackListAddLowercase_clicked();
    void on_pushButtonOcrBlackListAddUppercase_clicked();
    void on_pushButtonOcrBlackListAddNumbers_clicked();
    void on_labelOcrOptions1Reset_linkActivated(const QString &link);
    void on_comboBoxReplaceLang_currentTextChanged(const QString &arg1);
    void on_pushButtonReplaceAddRows_clicked();
    void on_pushButtonReplaceClearSelection_clicked();
    void on_labelOutputReset_linkActivated(const QString &link);

    void on_labelOcrOptions2Reset_linkActivated(const QString &link);

    void on_pushButtonOcrTesseractConfigFile_clicked();

    void on_pushButtonOutputLoggingFile_clicked();

    void on_comboBoxSpeechLang_currentTextChanged(const QString &text);

    void on_labelSpeechReset_linkActivated(const QString &);

    void on_pushButtonSpeechPreview_clicked();

protected:
    void showEvent(QShowEvent *event);

private:
    QColor pickColor(QColor initialColor);
    QFont pickFont(QFont initialFont);
    void setReplacementTable(QString lang);
    void saveReplacementTable(QString lang);
    void saveTranslateTable();
    void populateTranslateTable();
    QStringList getTranslateLangsSansOcrLang(QString ocrLang, QStringList translateLangs);
    void saveSpeechInfo(QString lang);
    void setSpeechInfo(QString lang);

    QString lastReplacementLang;
    QString lastSpeechLang;
    Ui::SettingsDialog *ui;

};

#endif // SETTINGSDIALOG_H
