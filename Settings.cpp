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

#include <QTextToSpeech>

#include "Settings.h"
#include "PostProcess.h"

const QColor Settings::defaultCaptureBoxBackgroundColor(0, 128, 255, 60);
const QColor Settings::defaultCaptureBoxBorderColor(0, 128, 255, 255);

const QString Settings::defaultHotkeyCaptureBox("Win+Q");
const QString Settings::defaultHotkeyReCaptureLast("Win+R");
const QString Settings::defaultHotkeyTextLineCapture("Win+E");
const QString Settings::defaultHotkeyForwardTextLineCapture("Win+W");
const QString Settings::defaultHotkeyBubbleCapture("Win+S");
const QString Settings::defaultHotkeyLang1("Win+1");
const QString Settings::defaultHotkeyLang2("Win+2");
const QString Settings::defaultHotkeyLang3("Win+3");
const QString Settings::defaultHotkeyTextOrientation("Win+O");
const QString Settings::defaultHotkeyWhitelist("Win+<Unmapped>");
const QString Settings::defaultHotkeyBlacklist("Win+<Unmapped>");

const QString Settings::defaultOcrLang("English");
const QString Settings::defaultOcrQuickAccessLang1("English");
const QString Settings::defaultOcrQuickAccessLang2("Japanese");
const QString Settings::defaultOcrQuickAccessLang3("German");

const QString Settings::defaultOcrTextOrientation("Auto");
const QString Settings::defaultOcrTesseractConfigFile("");
const QString Settings::defaultOcrWhitelist("");
const QString Settings::defaultOcrBlacklist("");
const double Settings::defaultOcrScaleFactor(3.5);

const QString Settings::defaultOutputLogFile("");
const QString Settings::defaultOutputLogFormat("${capture}${linebreak}");
const QFont Settings::defaultOutputPopupFont("Arial", 12);

const QColor Settings::defaultPreviewBackgroundColor(10, 10, 10, 255);
const QColor Settings::defaultPreviewBorderColor(128, 128, 128, 255);
const QString Settings::defaultPreviewPosition("Fixed - Top Left");
const QColor Settings::defaultPreviewTextColor(200, 200, 200, 255);
const QFont Settings::defaultPreviewTextFont("Arial", 16);

QList<Replacement> Settings::getOcrReplacementList(QString lang)
{
    static QList<Replacement> defaultJapaneseList = QList<Replacement>()
            << Replacement("カゝ", "か")
            << Replacement("ヵ", "カ")
            << Replacement("〈", "く")
            << Replacement("<", "く")
            << Replacement("＜", "く")
            << Replacement("︿", "く")
            << Replacement("\\|二", "に")
            << Replacement("_", "一")
            << Replacement("I", "ー")
            << Replacement("1", "ー")
            << Replacement("ムロ", "合")
            << Replacement("ぁ", "あ")
            << Replacement("ぃ", "い")
            << Replacement("ぅ", "う")
            << Replacement("ぇ", "え")
            << Replacement("ぉ", "お")
            << Replacement("ゎ", "わ")
            << Replacement("ァ", "ア")
            << Replacement("ィ", "イ")
            << Replacement("ゥ", "ウ")
            << Replacement("ェ", "エ")
            << Replacement("ォ", "オ")
            << Replacement("ヶ", "ケ")
            << Replacement("ヮ", "ワ")
               ;

    QString defaultValue = "";

    if(lang == "Japanese")
    {
        defaultValue = PostProcess::replacementListToStr(defaultJapaneseList);
    }

    return PostProcess::strToReplacementList(QSettings().value("Replacement/" + lang, defaultValue).toString());
}

void Settings::setOcrReplacementList(QString lang, QList<Replacement> value)
{
    QSettings().setValue("Replacement/" + lang, PostProcess::replacementListToStr(value));
}

QString Settings::getTranslateLang(QString ocrLang)
{
    QString defaultValue = "English";

    if(ocrLang == "English")
    {
        defaultValue = "German";
    }

    return QSettings().value("Translate/" + ocrLang, defaultValue).toString();
}

void Settings::setTranslateLang(QString ocrLang, QString translateLang)
{
    QSettings().setValue("Translate/" + ocrLang, translateLang);
}

void Settings::getSpeechInfo(QString ocrLang, QString &locale, QString &voice, int &rate, int &pitch)
{
    locale = QSettings().value("Speech/" + ocrLang + "/Locale", "<Init>").toString();
    voice = QSettings().value("Speech/" + ocrLang + "/Voice", "<Init>").toString();
    rate = QSettings().value("Speech/" + ocrLang + "/Rate", defaultSpeechRate).toInt();
    pitch = QSettings().value("Speech/" + ocrLang + "/Pitch", defaultSpeechPitch).toInt();

    if(voice == "<Init>")
    {
        // Default voice to disabled
        locale = "<Disabled>";
        voice = "<Disabled>";

        QTextToSpeech speech;
        QVector<QLocale> locales = speech.availableLocales();

        // Try to match up voice based on OCR langauge
        foreach (const QLocale &localeItem, locales)
        {
            speech.setLocale(localeItem);

            QString localLangName = QLocale::languageToString(localeItem.language());

            if(localLangName.startsWith(ocrLang))
            {
                auto voiceList = speech.availableVoices();

                foreach (const QVoice &voiceItem, voiceList)
                {
                    locale = localeItem.name();
                    voice = voiceItem.name();
                    break;
                }
            }
        }

        setSpeechInfo(ocrLang, locale, voice, rate, pitch);
    }
}

void Settings::setSpeechInfo(QString ocrLang, QString locale, QString voice, int rate, int pitch)
{
    QSettings().setValue("Speech/" + ocrLang + "/Locale", locale);
    QSettings().setValue("Speech/" + ocrLang + "/Voice", voice);
    QSettings().setValue("Speech/" + ocrLang + "/Rate", rate);
    QSettings().setValue("Speech/" + ocrLang + "/Pitch", pitch);
}

QString Settings::separatorToStr(QString separator)
{
    if(separator == "Space") return " ";
    else if(separator == "Tab") return "\t";
    else if(separator == "Line Break") return "\r\n";
    else if(separator == "Comma") return ",";
    else if(separator == "Semicolon") return ";";
    else if(separator == "Pipe") return "|";
    else return " ";
}
