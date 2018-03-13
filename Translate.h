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

#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QObject>
#include <QString>
#include <QMap>

class Translate : public QObject
{
    Q_OBJECT
public:
    Translate();
    bool startTranslate(QString keyword, QString from, QString to, int timeoutMillisec);
    static QStringList getAvailableLangs();
    static bool isValidLang(QString lang);

signals:
    void translationComplete(QString phrase, QString translation, bool error);

private:
    static QMap<QString, QString> populateLangMap();
    void requestFinished(QNetworkReply *data);

    static const QMap<QString, QString> mapLang; // Key = Lang name, Value = Two-letter code
    QNetworkAccessManager manager;
    QMap<QString, QString> requestMap; // Key = URL, Value = phrase.
};

#endif // TRANSLATE_H
