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

#include <QRegularExpression>
#include "PostProcess.h"

PostProcess::PostProcess(QString _ocrLang, bool _keepLineBreaks)
    : ocrLang(_ocrLang),
      keepLineBreaks(_keepLineBreaks)
{

}

QString PostProcess::postProcessOcrText(QString text)
{
    if(!keepLineBreaks)
    {
        if(ocrLang == "Japanese"
                || ocrLang == "Chinese - Simplified"
                || ocrLang == "Chinese - Traditional")
        {
            text.replace("\n", "");
            text.replace("\r", "");
        }
        else
        {
            text.replace("\r\n", " ");
            text.replace("\n", " ");
            text.replace("\r", " ");
        }
    }

    text = text.trimmed();

    for(auto item : replacementList)
    {
        text.replace(QRegularExpression(item.from), item.to);
    }

    return text;
}

QString PostProcess::replacementListToStr(QList<Replacement> list)
{
    QString str;

    for(auto item : list)
    {
        str += item.from + "\x1F" + item.to + "\x1F";
    }

    return str;
}


QList<Replacement> PostProcess::strToReplacementList(QString str)
{
    QList<Replacement> list;
    QStringList fields = str.split("\x1F",  QString::KeepEmptyParts);

    Replacement replacement;
    bool addFirst = true;

    for(auto item : fields)
    {
        if(addFirst)
        {
            replacement.from = item;
        }
        else
        {
            replacement.to = item;
            list.append(replacement);
        }

        addFirst = !addFirst;
    }

    return list;
}

void PostProcess::setReplacementList(const QList<Replacement> &value)
{
    replacementList = value;
}



