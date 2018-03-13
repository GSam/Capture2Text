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


#include <QtNetwork/QNetworkRequest>

#include <QThread>
#include <QDebug>
#include <QCoreApplication>
#include <QRegularExpressionMatchIterator>
#include "Translate.h"
#include "ReplyTimeout.h"

Translate::Translate()
{
    connect(&manager, &QNetworkAccessManager::finished, this, &Translate::requestFinished);
}

void Translate::requestFinished(QNetworkReply *reply)
{
    QString origPhrase = "";
    QString replyUrlStr = reply->request().url().toString();

    if(requestMap.contains(replyUrlStr))
    {
        origPhrase = requestMap[replyUrlStr];
        requestMap.remove(replyUrlStr);
    }

    if(!reply->isOpen())
    {
        qDebug() << "Translation timeout occured!";
        emit translationComplete(origPhrase, "", true);

        reply->deleteLater();
        return;
    }

    QString translationResponse(reply->readAll());
    reply->close();

    reply->deleteLater();

    // Example multi-line response (input was "Cat\nDog\nHorse"):
    // [[["Hund\n","Dog\n",null,null,1],["Katze\n","Cat\n",null,null,2],["Pferd","Horse",null,null,2]],null,"en"]

    // In case original phrase had double quotes, temporarily replace them with STX char
    translationResponse = translationResponse.replace("\\\"", QChar(2));

    QRegularExpression re("\\[\"(.*?)\"");
    QRegularExpressionMatchIterator i = re.globalMatch(translationResponse);
    QString translation;

    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        translation += match.captured(1);
    }

    translation = translation.replace("\\n", "\n");
    translation = translation.replace(QChar(2), "\"");

    //qDebug() << "Translation: " << translation;
    emit translationComplete(origPhrase, translation, false);
}


// https://stackoverflow.com/questions/8550147/how-to-use-google-translate-api-with-c/18813418#18813418
bool Translate::startTranslate(QString phrase, QString from, QString to, int timeoutMillisec=2000)
{
    QString langCodeFrom;
    QString langCodeTo;

    if(mapLang.contains(from))
    {
        langCodeFrom = mapLang[from];
    }
    else
    {
        return false;
    }

    if(mapLang.contains(to))
    {
        langCodeTo = mapLang[to];
    }
    else
    {
        return false;
    }

    // Example request:
    //   http://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl=ja&dt=t&q=hello
    // Answer:
    //   [[["こんにちは","hello",,,1]],,"en",,,,1,,[["en"],,[1],["en"]]]

    QString url = QString("http://translate.googleapis.com/translate_a/single?client=gtx&sl=%1&tl=%2&dt=t&q=%3")
            .arg(langCodeFrom, langCodeTo, phrase);

    // For debug, here is a URL that will timeout
    //QString url = QString("http://wiki.forum.nokia.com/index.php/How_to_wait_synchronously_for_a_Signal_in_Qt");

    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    requestMap.insert(reply->request().url().toString(), phrase);
    ReplyTimeout::set(reply, timeoutMillisec);
    //qDebug() << "Starting translation...";

    return true;
}

QStringList Translate::getAvailableLangs()
{
    QStringList list;

    for (auto item : mapLang.keys())
    {
        list.append(item);
    }

    list.sort();

    return list;
}

bool Translate::isValidLang(QString lang)
{
    return mapLang.contains(lang);
}

QMap<QString, QString> Translate::populateLangMap()
{
    QMap<QString, QString> map;

    map.insert("Afrikaans", "af");
    map.insert("Albanian", "sq");
    map.insert("Arabic", "ar");
    map.insert("Azerbaijani", "az");
    map.insert("Basque", "eu");
    map.insert("Bengali", "bn");
    map.insert("Belarusian", "be");
    map.insert("Bulgarian", "bg");
    map.insert("Catalan", "ca");
    map.insert("Chinese - Simplified", "zh-CN");
    map.insert("Chinese - Traditional", "zh-TW");
    map.insert("Croatian", "hr");
    map.insert("Czech", "cs");
    map.insert("Danish", "da");
    map.insert("Dutch", "nl");
    map.insert("English", "en");
    map.insert("Esperanto", "eo");
    map.insert("Estonian", "et");
    map.insert("Filipino", "tl");
    map.insert("Finnish", "fi");
    map.insert("French", "fr");
    map.insert("Galician", "gl");
    map.insert("Georgian", "ka");
    map.insert("German", "de");
    map.insert("Greek", "el");
    map.insert("Gujarati", "gu");
    map.insert("Haitian", "ht");
    map.insert("Hebrew", "iw");
    map.insert("Hindi", "hi");
    map.insert("Hungarian", "hu");
    map.insert("Icelandic", "is");
    map.insert("Indonesian", "id");
    map.insert("Irish", "ga");
    map.insert("Italian", "it");
    map.insert("Japanese", "ja");
    map.insert("Kannada", "kn");
    map.insert("Korean", "ko");
    map.insert("Latin", "la");
    map.insert("Latvian", "lv");
    map.insert("Lithuanian", "lt");
    map.insert("Macedonian", "mk");
    map.insert("Malay", "ms");
    map.insert("Maltese", "mt");
    map.insert("Norwegian", "no");
    map.insert("Persian", "fa");
    map.insert("Polish", "pl");
    map.insert("Portuguese", "pt");
    map.insert("Romanian", "ro");
    map.insert("Russian", "ru");
    map.insert("Serbian", "sr");
    map.insert("Slovak", "sk");
    map.insert("Slovenian", "sl");
    map.insert("Spanish", "es");
    map.insert("Swahili", "sw");
    map.insert("Swedish", "sv");
    map.insert("Tamil", "ta");
    map.insert("Telugu", "te");
    map.insert("Thai", "th");
    map.insert("Turkish", "tr");
    map.insert("Ukrainian", "uk");
    map.insert("Urdu", "ur");
    map.insert("Vietnamese", "vi");
    map.insert("Welsh", "cy");
    map.insert("Yiddish", "yi");

    return map;
}

const QMap<QString, QString> Translate::mapLang = populateLangMap();


