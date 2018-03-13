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

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include "OcrEngine.h"

OcrEngine::OcrEngine()
    : lang("English"),
      whitelist(""),
      blacklist(""),
      configFile("")
{
    populateLangMap();
    tessApi = new tesseract::TessBaseAPI();
}

OcrEngine::~OcrEngine()
{
    tessApi->End();
    delete tessApi;
}

QStringList OcrEngine::getInstalledLangs()
{
    QString exeDirpath = QCoreApplication::applicationDirPath();
    QDir dir(exeDirpath + QDir::separator() + "tessdata");
    QStringList nameFilter("*.traineddata");
    QStringList langFiles = dir.entryList(nameFilter);
    QStringList nameList;

    foreach (const QString &file, langFiles)
    {
        QString langCode = file;
        langCode.replace(".traineddata", "");

        if(mapCode.contains(langCode))
        {
            nameList.append(mapCode.value(langCode));
        }
    }

    nameList.sort();

    return nameList;
}

bool OcrEngine::isLangInstalled(QString lang)
{
    QStringList installedLangs = getInstalledLangs();
    return installedLangs.contains(lang);
}

bool OcrEngine::isLangCodeInstalled(QString langCode)
{
    QString exeDirpath = QCoreApplication::applicationDirPath();
    QDir dir(exeDirpath + QDir::separator() + "tessdata");
    QStringList nameFilter("*.traineddata");
    QStringList langFiles = dir.entryList(nameFilter);

    foreach (const QString &file, langFiles)
    {
        QString langCodeInDir = file;
        langCodeInDir.replace(".traineddata", "");

        if(langCodeInDir == langCode)
        {
            return true;
        }
    }

    return false;
}

QString OcrEngine::getFirstInstalledLang()
{
    QStringList installedLangs = getInstalledLangs();

    if(installedLangs.length() > 0)
    {
        return installedLangs[0];
    }

    return "None";
}

bool OcrEngine::setLang(QString lang)
{
    this->lang = lang;

    if(!mapLang.contains(lang))
    {
        return false;
    }

    if(!isLangInstalled(lang))
    {
        return false;
    }

    mutex.lock();

    tessApi->End();

    QString langCode = mapLang.value(lang);

    // As of Tesseract 4.0, horizontal and vertical dictionaries are
    // separate for languages that support vertical text.
    if(langCode == "chi_sim"
            || langCode == "chi_tra"
            || langCode == "jpn"
            || langCode == "kor"
            || langCode == "HanS"
            || langCode == "HanT"
            || langCode == "Hangul"
            )
    {
        QString vertLangCode = langCode + "_vert";

        if(isLangCodeInstalled(vertLangCode))
        {
            langCode += "+" + vertLangCode;
        }
    }

    QByteArray langCodeByteArray = langCode.toLocal8Bit();

    QString exeDirpath = QCoreApplication::applicationDirPath() + QDir::separator();

    if (tessApi->Init(exeDirpath.toLocal8Bit().constData(), langCodeByteArray.constData()))
    {
        qDebug() << "Unable to initialize OCR language: " << lang;
        mutex.unlock();
        return false;
    }

    mutex.unlock();

    return true;
}

QString OcrEngine::performOcr(PIX *pixs, bool singleTextLine)
{
    mutex.lock();

    tessApi->SetImage(pixs);

    if(verticalOrientation)
    {
        tessApi->SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_BLOCK_VERT_TEXT);

        // For Japanese and Chinese apply configuration that will improve accuracy
        if (lang == "Japanese"
                || lang == "Chinese - Simplified"
                || lang == "Chinese - Traditional")
        {
            bool status = true;
            status =  tessApi->SetVariable("tessedit_enable_dict_correction",  "1"     );
            status &= tessApi->SetVariable("textord_really_old_xheight",       "1"     );
            status &= tessApi->SetVariable("tosp_threshold_bias2",             "1"     );
            status &= tessApi->SetVariable("classify_norm_adj_midpoint",       "96"    );
            status &= tessApi->SetVariable("tessedit_class_miss_scale",        "0.002" );
            status &= tessApi->SetVariable("textord_initialx_ile",             "1.0"   );

            if(singleTextLine)
            {
                status &= tessApi->SetVariable("textord_min_linesize",         "2.5"   );
            }
            else
            {
                // For higher values of textord_min_linesize, Tesseract will get confused when lines are close together
                status &= tessApi->SetVariable("textord_min_linesize",         "2.0"   );
            }

            if(!status)
            {
                qDebug() << "performOCR: SetVariable failed!";
            }
        }
    }
    else
    {
        if(singleTextLine)
        {
            tessApi->SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_LINE);
        }
        else
        {
            tessApi->SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_BLOCK);
        }
    }

    tessApi->SetVariable("tessedit_char_whitelist", whitelist.toLocal8Bit().constData());
    tessApi->SetVariable("tessedit_char_blacklist", blacklist.toLocal8Bit().constData());

    if(configFile.length() > 0 && QFile::exists(configFile))
    {
        tessApi->ReadConfigFile(configFile.toLocal8Bit().constData());
    }

    char *outText = tessApi->GetUTF8Text();
    QString ocrText(outText);

    tessApi->Clear();

    delete [] outText;

    mutex.unlock();

    return ocrText;
}

QString OcrEngine::altLangToLang(QString ocrLang)
{
    if(mapLang.contains(ocrLang))
    {
        return ocrLang;
    }
    else if(mapLangAlt.contains(ocrLang))
    {
        return mapLangAlt[ocrLang];
    }

    return "None";
}

QMap<QString, QString> OcrEngine::populateLangMap()
{
    QMap<QString, QString> map;

    map.insert("Afrikaans", "afr");
    map.insert("Albanian", "sqi");
    map.insert("Amharic", "amh");
    map.insert("Ancient Greek", "grc");
    map.insert("Arabic", "ara");
    map.insert("Assamese", "asm");
    map.insert("Azerbaijani (Alternate)", "aze_cyrl");
    map.insert("Azerbaijani", "aze");
    map.insert("Basque", "eus");
    map.insert("Belarusian", "bel");
    map.insert("Bengali", "ben");
    map.insert("Bosnian", "bos");
    map.insert("Bulgarian", "bul");
    map.insert("Burmese", "mya");
    map.insert("Catalan", "cat");
    map.insert("Cebuano", "ceb");
    map.insert("Central Khmer", "khm");
    map.insert("Cherokee", "chr");
    map.insert("Chinese - Simplified", "chi_sim");
    map.insert("Chinese - Traditional", "chi_tra");
    map.insert("Croatian", "hrv");
    map.insert("Czech", "ces");
    map.insert("Danish (Alternate)", "dan_frak");
    map.insert("Danish", "dan");
    map.insert("Dutch", "nld");
    map.insert("Dzongkha", "dzo");
    map.insert("English", "eng");
    map.insert("Esperanto", "epo");
    map.insert("Estonian", "est");
    map.insert("Finnish", "fin");
    map.insert("Frankish", "frk");
    map.insert("French", "fra");
    map.insert("Galician", "glg");
    map.insert("Georgian (Old)", "kat_old");
    map.insert("Georgian", "kat");
    map.insert("German (Alternate)", "deu_frak");
    map.insert("German", "deu");
    map.insert("Greek", "ell");
    map.insert("Gujarati", "guj");
    map.insert("Haitian", "hat");
    map.insert("Hebrew", "heb");
    map.insert("Hindi", "hin");
    map.insert("Hungarian", "hun");
    map.insert("Icelandic", "isl");
    map.insert("Indic", "inc");
    map.insert("Indonesian", "ind");
    map.insert("Inuktitut", "iku");
    map.insert("Irish", "gle");
    map.insert("Italian (Old)", "ita_old");
    map.insert("Italian", "ita");
    map.insert("Japanese", "jpn");
    map.insert("Javanese", "jav");
    map.insert("Kannada", "kan");
    map.insert("Kazakh", "kaz");
    map.insert("Kirghiz", "kir");
    map.insert("Korean", "kor");
    map.insert("Kurukh", "kru");
    map.insert("Lao", "lao");
    map.insert("Latin", "lat");
    map.insert("Latvian", "lav");
    map.insert("Lithuanian", "lit");
    map.insert("Macedonian", "mkd");
    map.insert("Malay", "msa");
    map.insert("Malayalam", "mal");
    map.insert("Maltese", "mlt");
    map.insert("Marathi", "mar");
    map.insert("Math/Equations", "equ");
    map.insert("Middle English (1100-1500)", "enm");
    map.insert("Middle French (1400-1600)", "frm");
    map.insert("Nepali", "nep");
    map.insert("Norwegian", "nor");
    map.insert("Odiya", "ori");
    map.insert("Panjabi", "pan");
    map.insert("Persian", "fas");
    map.insert("Polish", "pol");
    map.insert("Portuguese", "por");
    map.insert("Pushto", "pus");
    map.insert("Romanian", "ron");
    map.insert("Russian", "rus");
    map.insert("Sanskrit", "san");
    map.insert("Serbian", "srp");
    map.insert("Sinhala", "sin");
    map.insert("Slovak (Alternate)", "slk_frak");
    map.insert("Slovak", "slk");
    map.insert("Slovenian", "slv");
    map.insert("Spanish (Old)", "spa_old");
    map.insert("Spanish", "spa");
    map.insert("srp_latn", "srp_latn");
    map.insert("Swahili", "swa");
    map.insert("Swedish", "swe");
    map.insert("Syriac", "syr");
    map.insert("Tagalog", "tgl");
    map.insert("Tajik", "tgk");
    map.insert("Tamil", "tam");
    map.insert("Telugu", "tel");
    map.insert("Thai", "tha");
    map.insert("Tibetan", "bod");
    map.insert("Tigrinya", "tir");
    map.insert("Turkish", "tur");
    map.insert("Uighur", "uig");
    map.insert("Ukrainian", "ukr");
    map.insert("Urdu", "urd");
    map.insert("Uzbek (Alternate)", "uzb_cyrl");
    map.insert("Uzbek", "uzb");
    map.insert("Vietnamese", "vie");
    map.insert("Welsh", "cym");
    map.insert("Yiddish", "yid");

    return map;
}

QMap<QString, QString> OcrEngine::populateCodeMap()
{
    QMap<QString, QString> map;

    for(auto key : mapLang.keys())
    {
        map.insert(mapLang[key], key);
    }

    return map;
}

QMap<QString, QString> OcrEngine::populateAltLangMap()
{
    QMap<QString, QString> map;

    map.insert("Danish (Alternate)", "Danish");
    map.insert("Georgian (Old)", "Georgian");
    map.insert("German (Alternate)", "German");
    map.insert("Italian (Old)", "Italian");
    map.insert("Middle English (1100-1500)", "English");
    map.insert("Middle French (1400-1600)", "French");
    map.insert("Slovak (Alternate)", "Slovak");
    map.insert("Spanish (Old)", "Spanish");

    return map;
}

const QMap<QString, QString> OcrEngine::mapLang = populateLangMap();
const QMap<QString, QString> OcrEngine::mapCode = populateCodeMap();
const QMap<QString, QString> OcrEngine::mapLangAlt = populateAltLangMap();
