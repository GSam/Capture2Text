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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>
#include <QCoreApplication>
#include <QFont>
#include <QSettings>
#include <QSize>
#include <QTextToSpeech>

#include "Hotkey.h"
#include "PostProcess.h"

class Settings
{
public:
    static const QColor defaultCaptureBoxBackgroundColor;
    static QColor getCaptureBoxBackgroundColor() { return QSettings().value("CaptureBox/BackgroundColor", defaultCaptureBoxBackgroundColor).value<QColor>(); }
    static void setCaptureBoxBackgroundColor( QColor value) { QSettings().setValue("CaptureBox/BackgroundColor", value); }

    static const QColor defaultCaptureBoxBorderColor;
    static QColor getCaptureBoxBorderColor() { return QSettings().value("CaptureBox/BorderColor", defaultCaptureBoxBorderColor).value<QColor>(); }
    static void setCaptureBoxBorderColor( QColor value) { QSettings().setValue("CaptureBox/BorderColor", value); }

    static const bool defaultDebugPrependCoords = false;
    static bool getDebugPrependCoords() { return QSettings().value("Debug/PrependCoords", defaultDebugPrependCoords).toBool(); }
    static void setDebugPrependCoords(bool value) { QSettings().setValue("Debug/PrependCoords", value); }

    static const bool defaultDebugSaveCaptureImage = false;
    static bool getDebugSaveCaptureImage() { return QSettings().value("Debug/SaveCaptureImage", defaultDebugSaveCaptureImage).toBool(); }
    static void setDebugSaveCaptureImage(bool value) { QSettings().setValue("Debug/SaveCaptureImage", value); }

    static const bool defaultDebugSaveEnhancedImage = false;
    static bool getDebugSaveEnhancedImage() { return QSettings().value("Debug/SaveEnhancedImage", defaultDebugSaveEnhancedImage).toBool(); }
    static void setDebugSaveEnhancedImage(bool value) { QSettings().setValue("Debug/SaveEnhancedImage", value); }

    static const bool defaultDebugAppendTimestampToImage = false;
    static bool getDebugAppendTimestampToImage() { return QSettings().value("Debug/AppendTimestampToImage", defaultDebugAppendTimestampToImage).toBool(); }
    static void setDebugAppendTimestampToImage(bool value) { QSettings().setValue("Debug/AppendTimestampToImage", value); }

    static const QString defaultHotkeyCaptureBox;
    static QString getHotkeyCaptureBox() { return QSettings().value("Hotkey/CaptureBox", defaultHotkeyCaptureBox).toString(); }
    static void setHotkeyCaptureBox(QString value) { QSettings().setValue("Hotkey/CaptureBox", value); }

    static const QString defaultHotkeyReCaptureLast;
    static QString getHotkeyReCaptureLast() { return QSettings().value("Hotkey/ReCaptureLast", defaultHotkeyReCaptureLast).toString(); }
    static void setHotkeyReCaptureLast(QString value) { QSettings().setValue("Hotkey/ReCaptureLast", value); }

    static const QString defaultHotkeyTextLineCapture;
    static QString getHotkeyTextLineCapture() { return QSettings().value("Hotkey/TextLineCapture", defaultHotkeyTextLineCapture).toString(); }
    static void setHotkeyTextLineCapture(QString value) { QSettings().setValue("Hotkey/TextLineCapture", value); }

    static const QString defaultHotkeyForwardTextLineCapture;
    static QString getHotkeyForwardTextLineCapture() { return QSettings().value("Hotkey/ForwardTextLineCapture", defaultHotkeyForwardTextLineCapture).toString(); }
    static void setHotkeyForwardTextLineCapture(QString value) { QSettings().setValue("Hotkey/ForwardTextLineCapture", value); }

    static const QString defaultHotkeyBubbleCapture;
    static QString getHotkeyBubbleCapture() { return QSettings().value("Hotkey/BubbleCapture", defaultHotkeyBubbleCapture).toString(); }
    static void setHotkeyBubbleCapture(QString value) { QSettings().setValue("Hotkey/BubbleCapture", value); }

    static const QString defaultHotkeyLang1;
    static QString getHotkeyLang1() { return QSettings().value("Hotkey/Lang1", defaultHotkeyLang1).toString(); }
    static void setHotkeyLang1(QString value) { QSettings().setValue("Hotkey/Lang1", value); }

    static const QString defaultHotkeyLang2;
    static QString getHotkeyLang2() { return QSettings().value("Hotkey/Lang2", defaultHotkeyLang2).toString(); }
    static void setHotkeyLang2(QString value) { QSettings().setValue("Hotkey/Lang2", value); }

    static const QString defaultHotkeyLang3;
    static QString getHotkeyLang3() { return QSettings().value("Hotkey/Lang3", defaultHotkeyLang3).toString(); }
    static void setHotkeyLang3(QString value) { QSettings().setValue("Hotkey/Lang3", value); }

    static const QString defaultHotkeyTextOrientation;
    static QString getHotkeyTextOrientation() { return QSettings().value("Hotkey/TextOrientation", defaultHotkeyTextOrientation).toString(); }
    static void setHotkeyTextOrientation(QString value) { QSettings().setValue("Hotkey/TextOrientation", value); }

    static const QString defaultHotkeyWhitelist;
    static QString getHotkeyWhitelist() { return QSettings().value("Hotkey/Whitelist", defaultHotkeyWhitelist).toString(); }
    static void setHotkeyWhitelist(QString value) { QSettings().setValue("Hotkey/Whitelist", value); }

    static const QString defaultHotkeyBlacklist;
    static QString getHotkeyBlacklist() { return QSettings().value("Hotkey/Blacklist", defaultHotkeyBlacklist).toString(); }
    static void setHotkeyBlacklist(QString value) { QSettings().setValue("Hotkey/Blacklist", value); }

    static const QString defaultOcrLang;
    static QString getOcrLang() { return QSettings().value("OCR/Language", defaultOcrLang).toString(); }
    static void setOcrLang(QString value) { QSettings().setValue("OCR/Language", value); }

    static const QString defaultOcrQuickAccessLang1;
    static QString getOcrQuickAccessLang1() { return QSettings().value("OCR/QuickAccessLang1", defaultOcrQuickAccessLang1).toString(); }
    static void setOcrQuickAccessLang1(QString value) { QSettings().setValue("OCR/QuickAccessLang1", value); }

    static const QString defaultOcrQuickAccessLang2;
    static QString getOcrQuickAccessLang2() { return QSettings().value("OCR/QuickAccessLang2", defaultOcrQuickAccessLang2).toString(); }
    static void setOcrQuickAccessLang2(QString value) { QSettings().setValue("OCR/QuickAccessLang2", value); }

    static const QString defaultOcrQuickAccessLang3;
    static QString getOcrQuickAccessLang3() { return QSettings().value("OCR/QuickAccessLang3", defaultOcrQuickAccessLang3).toString(); }
    static void setOcrQuickAccessLang3(QString value) { QSettings().setValue("OCR/QuickAccessLang3", value); }

    static const bool defaultOcrEnableWhitelist = false;
    static bool getOcrEnableWhitelist() { return QSettings().value("OCR/EnableWhitelist", defaultOcrEnableWhitelist).toBool(); }
    static void setOcrEnableWhitelist(bool value) { QSettings().setValue("OCR/EnableWhitelist", value); }

    static const QString defaultOcrWhitelist;
    static QString getOcrWhitelist() { return QSettings().value("OCR/Whitelist", defaultOcrWhitelist).toString(); }
    static void setOcrWhitelist(QString value) { QSettings().setValue("OCR/Whitelist", value); }

    static const bool defaultOcrEnableBlacklist = false;
    static bool getOcrEnableBlacklist() { return QSettings().value("OCR/EnableBlacklist", defaultOcrEnableBlacklist).toBool(); }
    static void setOcrEnableBlacklist(bool value) { QSettings().setValue("OCR/EnableBlacklist", value); }

    static const QString defaultOcrBlacklist;
    static QString getOcrBlacklist() { return QSettings().value("OCR/Blacklist", defaultOcrBlacklist).toString(); }
    static void setOcrBlacklist(QString value) { QSettings().setValue("OCR/Blacklist", value); }

    static const QString defaultOcrTextOrientation;
    static QString getOcrTextOrientation() { return QSettings().value("OCR/TextOrientation", defaultOcrTextOrientation).toString(); }
    static void setOcrTextOrientation(QString value) { QSettings().setValue("OCR/TextOrientation", value); }

    static const QString defaultOcrTesseractConfigFile;
    static QString getOcrTesseractConfigFile() { return QSettings().value("OCR/TesseractConfigFile", defaultOcrTesseractConfigFile).toString(); }
    static void setOcrTesseractConfigFile(QString value) { QSettings().setValue("OCR/TesseractConfigFile", value); }

    static const double defaultOcrScaleFactor;
    static double getOcrScaleFactor() { return QSettings().value("OCR/ScaleFactor", defaultOcrScaleFactor).toDouble(); }
    static void setOcrScaleFactor(double value) { QSettings().setValue("OCR/ScaleFactor", value); }

    static const bool defaultOcrTrim = false;
    static bool getOcrTrim() { return QSettings().value("OCR/Trim", defaultOcrTrim).toBool(); }
    static void setOcrTrim(bool value) { QSettings().setValue("OCR/Trim", value); }

    static const bool defaultOcrDeskew = false;
    static bool getOcrDeskew() { return QSettings().value("OCR/Deskew", defaultOcrDeskew).toBool(); }
    static void setOcrDeskew(bool value) { QSettings().setValue("OCR/Deskew", value); }

    static const int defaultTextLineCaptureLength = 1500;
    static int getTextLineCaptureLength() { return QSettings().value("TextLineCapture/Length", defaultTextLineCaptureLength).toInt(); }
    static void setTextLineCaptureLength(int value) { QSettings().setValue("TextLineCapture/Length", value); }

    static const int defaultTextLineCaptureWidth = 70;
    static int getTextLineCaptureWidth() { return QSettings().value("TextLineCapture/Width", defaultTextLineCaptureWidth).toInt(); }
    static void setTextLineCaptureWidth(int value) { QSettings().setValue("TextLineCapture/Width", value); }

    static const int defaultTextLineCaptureLookahead = 14;
    static int getTextLineCaptureLookahead() { return QSettings().value("TextLineCapture/Lookahead", defaultTextLineCaptureLookahead).toInt(); }
    static void setTextLineCaptureLookahead(int value) { QSettings().setValue("TextLineCapture/Lookahead", value); }

    static const int defaultTextLineCaptureLookbehind = 14;
    static int getTextLineCaptureLookbehind() { return QSettings().value("TextLineCapture/Lookbehind", defaultTextLineCaptureLookbehind).toInt(); }
    static void setTextLineCaptureLookbehind(int value) { QSettings().setValue("TextLineCapture/Lookbehind", value); }

    static const int defaultTextLineCaptureSearchRadius = 30;
    static int getTextLineCaptureSearchRadius() { return QSettings().value("TextLineCapture/SearchRadius", defaultTextLineCaptureSearchRadius).toInt(); }
    static void setTextLineCaptureSearchRadius(int value) { QSettings().setValue("TextLineCapture/SearchRadius", value); }

    static const int defaultForwardTextLineCaptureLength = 750;
    static int getForwardTextLineCaptureLength() { return QSettings().value("ForwardTextLineCapture/Length", defaultForwardTextLineCaptureLength).toInt(); }
    static void setForwardTextLineCaptureLength(int value) { QSettings().setValue("ForwardTextLineCapture/Length", value); }

    static const int defaultForwardTextLineCaptureWidth = 70;
    static int getForwardTextLineCaptureWidth() { return QSettings().value("ForwardTextLineCapture/Width", defaultForwardTextLineCaptureWidth).toInt(); }
    static void setForwardTextLineCaptureWidth(int value) { QSettings().setValue("ForwardTextLineCapture/Width", value); }

    static const int defaultForwardTextLineCaptureLookahead = 14;
    static int getForwardTextLineCaptureLookahead() { return QSettings().value("ForwardTextLineCapture/Lookahead", defaultForwardTextLineCaptureLookahead).toInt(); }
    static void setForwardTextLineCaptureLookahead(int value) { QSettings().setValue("ForwardTextLineCapture/Lookahead", value); }

    static const int defaultForwardTextLineCaptureLookbehind = 1;
    static int getForwardTextLineCaptureLookbehind() { return QSettings().value("ForwardTextLineCapture/Lookbehind", defaultForwardTextLineCaptureLookbehind).toInt(); }
    static void setForwardTextLineCaptureLookbehind(int value) { QSettings().setValue("ForwardTextLineCapture/Lookbehind", value); }

    static const int defaultForwardTextLineCaptureStartOffset = 25;
    static int getForwardTextLineCaptureStartOffset() { return QSettings().value("ForwardTextLineCapture/StartOffset", defaultForwardTextLineCaptureStartOffset).toInt(); }
    static void setForwardTextLineCaptureStartOffset(int value) { QSettings().setValue("ForwardTextLineCapture/StartOffset", value); }

    static const int defaultForwardTextLineCaptureSearchRadius = 30;
    static int getForwardTextLineCaptureSearchRadius() { return QSettings().value("ForwardTextLineCapture/SearchRadius", defaultForwardTextLineCaptureSearchRadius).toInt(); }
    static void setForwardTextLineCaptureSearchRadius(int value) { QSettings().setValue("ForwardTextLineCapture/SearchRadius", value); }

    static const bool defaultForwardTextLineCaptureFirstWord = false;
    static bool getForwardTextLineCaptureFirstWord() { return QSettings().value("ForwardTextLineCapture/FirstWord", defaultForwardTextLineCaptureFirstWord).toBool(); }
    static void setForwardTextLineCaptureFirstWord(bool value) { QSettings().setValue("ForwardTextLineCapture/FirstWord", value); }

    static const int defaultBubbleCaptureWidth = 500;
    static int getBubbleCaptureWidth() { return QSettings().value("BubbleCapture/Width", defaultBubbleCaptureWidth).toInt(); }
    static void setBubbleCaptureWidth(int value) { QSettings().setValue("BubbleCapture/Width", value); }

    static const int defaultBubbleCaptureHeight = 500;
    static int getBubbleCaptureHeight() { return QSettings().value("BubbleCapture/Height", defaultBubbleCaptureHeight).toInt(); }
    static void setBubbleCaptureHeight(int value) { QSettings().setValue("BubbleCapture/Height", value); }

    static const bool defaultOutputKeepLineBreaks = true;
    static bool getOutputKeepLineBreaks() { return QSettings().value("Output/KeepLineBreaks", defaultOutputKeepLineBreaks).toBool(); }
    static void setOutputKeepLineBreaks(bool value) { QSettings().setValue("Output/KeepLineBreaks", value); }

    static const bool defaultOutputClipbaord = true;
    static bool getOutputClipboard() { return QSettings().value("Output/OutputClipboard", defaultOutputClipbaord).toBool(); }
    static void setOutputClipboard(bool value) { QSettings().setValue("Output/OutputClipboard", value); }

    static const QString defaultOutputLogFile;
    static QString getOutputLogFile() { return QSettings().value("Output/LogFile", defaultOutputLogFile).toString(); }
    static void setOutputLogFile(QString value) { QSettings().setValue("Output/LogFile", value); }

    static const bool defaultOutputLogFileEnable = false;
    static bool getOutputLogFileEnable() { return QSettings().value("Output/LogFileEnable", defaultOutputLogFileEnable).toBool(); }
    static void setOutputLogFileEnable(bool value) { QSettings().setValue("Output/LogFileEnable", value); }

    static const QString defaultOutputLogFormat;
    static QString getOutputLogFormat() { return QSettings().value("Output/LogFormat", defaultOutputLogFormat).toString(); }
    static void setOutputLogFormat(QString value) { QSettings().setValue("Output/LogFormat", value); }

    static QSize getOutputPopupWindowSize() { return QSettings().value("Output/OutputPopupWindowSize", QSize(355, 165)).toSize(); }
    static void setOutputPopupWindowSize(QSize value) { QSettings().setValue("Output/OutputPopupWindowSize", value); }

    static const QFont defaultOutputPopupFont;
    static QFont getOutputPopupFont() { return QSettings().value("Output/OutputPopupFont", defaultOutputPopupFont).value<QFont>(); }
    static void setOutputPopupFont( QFont value) { QSettings().setValue("Output/OutputPopupFont", value); }

    static const bool defaultOutputShowPopup = true;
    static bool getOutputShowPopup() { return QSettings().value("Output/OutputPopup", defaultOutputShowPopup).toBool(); }
    static void setOutputShowPopup(bool value) { QSettings().setValue("Output/OutputPopup", value); }

    static bool getOutputPopupTopmost() { return QSettings().value("Output/PopupTopmost", true).toBool(); }
    static void setOutputPopupTopmost(bool value) { QSettings().setValue("Output/PopupTopmost", value); }

    static const QColor defaultPreviewBackgroundColor;
    static QColor getPreviewBackgroundColor() { return QSettings().value("Preview/BackgroundColor", defaultPreviewBackgroundColor).value<QColor>(); }
    static void setPreviewBackgroundColor( QColor value) { QSettings().setValue("Preview/BackgroundColor", value); }

    static const QColor defaultPreviewBorderColor;
    static QColor getPreviewBorderColor() { return QSettings().value("Preview/BorderColor", defaultPreviewBorderColor).value<QColor>(); }
    static void setPreviewBorderColor( QColor value) { QSettings().setValue("Preview/BorderColor", value); }

    static const bool defaultPreviewEnabled = true;
    static bool getPreviewEnabled() { return QSettings().value("Preview/Enabled", defaultPreviewEnabled).toBool(); }
    static void setPreviewEnabled(bool value) { QSettings().setValue("Preview/Enabled", value); }

    static const QString defaultPreviewPosition;
    static QString getPreviewPosition() { return QSettings().value("Preview/Position", defaultPreviewPosition).toString(); }
    static void setPreviewPosition(QString value) { QSettings().setValue("Preview/Position", value); }

    static const QColor defaultPreviewTextColor;
    static QColor getPreviewTextColor() { return QSettings().value("Preview/TextColor", defaultPreviewTextColor).value<QColor>(); }
    static void setPreviewTextColor( QColor value) { QSettings().setValue("Preview/TextColor", value); }

    static const QFont defaultPreviewTextFont;
    static QFont getPreviewTextFont() { return QSettings().value("Preview/TextFont", defaultPreviewTextFont).value<QFont>(); }
    static void setPreviewTextFont( QFont value) { QSettings().setValue("Preview/TextFont", value); }

    static QList<Replacement> getOcrReplacementList(QString lang);
    static void setOcrReplacementList(QString lang, QList<Replacement> value);

    static const bool defaultTranslateAddToClipboard = false;
    static bool getTranslateAddToClipboard() { return QSettings().value("Translate/AddToClipboard", defaultTranslateAddToClipboard).toBool(); }
    static void setTranslateAddToClipboard(bool value) { QSettings().setValue("Translate/AddToClipboard", value); }

    static const bool defaultTranslateAddToPopup = false;
    static bool getTranslateAddToPopup() { return QSettings().value("Translate/AddToPopup", defaultTranslateAddToPopup).toBool(); }
    static void setTranslateAddToPopup(bool value) { QSettings().setValue("Translate/AddToPopup", value); }

    static QString getTranslateSeparator() { return QSettings().value("Translate/Separator", "Space").toString(); }
    static void setTranslateSeparator(QString value) { QSettings().setValue("Translate/Separator", value); }
    static QString separatorToStr(QString separator);

    static QString getTranslateLang(QString ocrLang);
    static void setTranslateLang(QString ocrLang, QString translateLang);

    static const int defaultTranslateServerTimeout = 2000;
    static int getTranslateServerTimeout() { return QSettings().value("Translate/ServerTimeout", defaultTranslateServerTimeout).toInt(); }
    static void setTranslateServerTimeout(int value) { QSettings().setValue("Translate/ServerTimeout", value); }

    static const bool defaultSpeechEnable = false;
    static bool getSpeechEnable() { return QSettings().value("Speech/Enable", defaultSpeechEnable).toBool(); }
    static void setSpeechEnable(bool value) { QSettings().setValue("Speech/Enable", value); }

    static const int defaultSpeechVolume = 70;
    static int getSpeechVolume() { return QSettings().value("Speech/Volume", defaultSpeechVolume).toInt(); }
    static void setSpeechVolume(int value) { QSettings().setValue("Speech/Volume", value); }

    static const int defaultSpeechRate = 0;
    static const int defaultSpeechPitch = 0;
    static void getSpeechInfo(QString ocrLang, QString &locale, QString &voice, int &rate, int &pitch);
    static void setSpeechInfo(QString ocrLang, QString locale, QString voice, int rate, int pitch);

    static bool getMiscShowWelcome() { return QSettings().value("Misc/ShowWelcome", true).toBool(); }
    static void setMiscShowWelcome(bool value) { QSettings().setValue("Misc/ShowWelcome", value); }

    static QString getMiscVersion() { return QSettings().value("Misc/Version", QCoreApplication::applicationVersion()).toString(); }
    static void setMiscVersion(QString value) { QSettings().setValue("Misc/Version", value); }

private:

    Settings() { }

};

#endif // SETTINGS_H
