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

#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <QString>
#include <QList>
#include <QPair>


struct Replacement
{
    QString from; // Regex
    QString to;
    Replacement() : from(""), to("") {}
    Replacement(QString _from, QString _to)
        : from(_from), to(_to) {}

};

class PostProcess
{
public:
    PostProcess(QString _ocrLang, bool _keepLineBreaks);
    QString postProcessOcrText(QString text);
    static QString replacementListToStr(QList<Replacement> list);
    static QList<Replacement> strToReplacementList(QString str);
    void setReplacementList(const QList<Replacement> &value);

private:
    QString ocrLang;
    bool keepLineBreaks;
    QList<Replacement> replacementList;

};

#endif // POSTPROCESS_H
