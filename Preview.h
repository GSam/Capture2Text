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

#ifndef PREVIEW_H
#define PREVIEW_H

#include <QtGui>
#include <QDialog>
#include <QFont>
#include <QTimer>

class Preview : public QWidget
{
        Q_OBJECT
public:
    Preview();
    void setText(const QString &text);
    QString getText();
    void setTextColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setBorderColor(const QColor &color);
    void setTextFont(const QFont &font);
    int getBoxHeight();
    void hideAndReset();
    void showInfo(QString text);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void infoTimeout();

    const int horizontalPadding = 10;
    const int verticalPadding = 6;

    QString previewText;
    QColor textColor;
    QColor backgroundColor;
    QColor borderColor;
    QFont textFont;
    QTimer infoTimer;
};

#endif // PREVIEW_H
