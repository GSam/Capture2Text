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

#ifndef OCR_ENGINE_H
#define OCR_ENGINE_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QMutex>
#include <QRect>

#include "baseapi.h"
#include "allheaders.h"


class OcrEngine
{
public:
    OcrEngine();
    ~OcrEngine();
public:
    static QStringList getInstalledLangs();
    static bool isLangInstalled(QString lang);
    static QString getFirstInstalledLang();
    bool setLang(QString lang);
    QString performOcr(PIX *pixs, bool singleLine);

    QString getLang() { return lang; }
    bool getVerticalOrientation() const { return verticalOrientation; }
    void setVerticalOrientation(bool value) { verticalOrientation = value; }

    static QString altLangToLang(QString ocrLang);

    QString getWhitelist() const { return whitelist; }
    void setWhitelist(const QString &value) { whitelist = value; }

    QString getBlacklist() const { return blacklist; }
    void setBlacklist(const QString &value) { blacklist = value; }

    QString getConfigFile() const { return configFile; }
    void setConfigFile(const QString &value) { configFile = value.trimmed(); }

private:
    bool isLangCodeInstalled(QString langCode);

    static QMap<QString, QString> populateLangMap();
    static QMap<QString, QString> populateCodeMap();
    static QMap<QString, QString> populateAltLangMap();

    QMutex mutex;
    QString lang;
    bool verticalOrientation = false;
    QString whitelist;
    QString blacklist;
    QString configFile;

    tesseract::TessBaseAPI *tessApi;
    static const QMap<QString, QString> mapLang; // Key = Lang name, Value = Tesseract Code
    static const QMap<QString, QString> mapCode; // Key = Tesseract Code, Value = Lang name
    static const QMap<QString, QString> mapLangAlt; // Key = Alt Lang name, Value = Lang name

};

#endif // OCR_ENGINE_H
