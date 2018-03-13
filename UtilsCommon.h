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

#ifndef UTILSCOMMON_H
#define UTILSCOMMON_H

#include <QDateTime>

#ifndef CLI_BUILD
    #include <QColor>
    #include <QFont>
#endif

class UtilsCommon
{
public:
    #ifndef CLI_BUILD
    static QColor pickColor(QColor initialColor);
    static QFont pickFont(QFont initialFont);
    #endif
    static QString timestampToStr(QDateTime timestamp);
    static QString getAppDir(bool appendSlash);
    static QString formatLogLine(QString format, QString ocrText, QDateTime timestamp, QString translation, QString file);
private:
    UtilsCommon() { }
};

#endif // UTILSCOMMON_H
