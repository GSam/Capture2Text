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
#include <QDir>

#ifndef CLI_BUILD
    #include <QColorDialog>
    #include <QFontDialog>
#endif

#include "UtilsCommon.h"

#ifndef CLI_BUILD
QColor UtilsCommon::pickColor(QColor initialColor)
{
    QColor color = QColorDialog::getColor(initialColor,
                                          nullptr, "Select Color", QColorDialog::ShowAlphaChannel);
    if(!color.isValid())
    {
        return initialColor;
    }

    return color;
}

QFont UtilsCommon::pickFont(QFont initialFont)
{
    bool fontValid = false;
    QFont font = QFontDialog::getFont(&fontValid, initialFont, nullptr, "Select Font");

    if(!fontValid)
    {
        return initialFont;
    }

    return font;
}
#endif


QString UtilsCommon::timestampToStr(QDateTime timestamp)
{
    return timestamp.toString("yyyyMMdd_hhmmsszzz");
}

QString UtilsCommon::getAppDir(bool appendSlash)
{
    if(appendSlash)
    {
        return QCoreApplication::applicationDirPath() + QDir::separator();
    }
    else
    {
        return QCoreApplication::applicationDirPath();
    }
}

QString UtilsCommon::formatLogLine(QString format, QString ocrText, QDateTime timestamp, QString translation, QString file)
{
    QString line = format;
    line.replace("${tab}", "\t");
    line.replace("${linebreak}", "\n");
    line.replace("${timestamp}", timestampToStr(timestamp));
    line.replace("${file}", file);
    line.replace("${translation}", translation);
    line.replace("${capture}", ocrText);
    return line;
}
