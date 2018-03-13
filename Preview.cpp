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

#include <QDesktopWidget>
#include <QApplication>
#include "Preview.h"

Preview::Preview()
    : QWidget(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::Tool),
      previewText(""),
      textColor(QColor(128, 128, 128, 255)),
      backgroundColor(QColor(8, 8, 8, 255)),
      borderColor(QColor(128, 128, 128, 255)),
      textFont(QFont("Arial", 16)),
      infoTimer(this)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(1, 1);

    connect(&infoTimer, &QTimer::timeout, this, &Preview::infoTimeout);
}

void Preview::infoTimeout()
{
    infoTimer.stop();
    hide();
}

void Preview::showInfo(QString text)
{
    setText(text);
    QFontMetrics metrics(textFont);
    QPoint pt = QApplication::desktop()->availableGeometry().bottomRight();
    pt.setX(pt.x() - metrics.width(previewText) - horizontalPadding);
    pt.setY(pt.y() - getBoxHeight());
    move(pt);
    show();
    raise();
    update();
    infoTimer.start(1500);
}

void Preview::setText(const QString &text)
{
    previewText = text;
}

QString Preview::getText()
{
    return previewText;
}

void Preview::setTextColor(const QColor &color)
{
    textColor = color;
}

void Preview::setBackgroundColor(const QColor &color)
{
    backgroundColor = color;
}

void Preview::setBorderColor(const QColor &color)
{
    borderColor = color;
}

void Preview::setTextFont(const QFont &font)
{
    textFont = font;
}

int Preview::getBoxHeight()
{
    QFontMetrics metrics(textFont);
    return metrics.height() + verticalPadding;
}

void Preview::hideAndReset()
{
    hide();
    previewText = "";
}

void Preview::paintEvent(QPaintEvent *)
{
    if(previewText == "")
    {
        return;
    }

    QPainter painter(this);

    // Fit dialog to text
    QDesktopWidget desktopWidget;
    QRect screenRect = desktopWidget.screenGeometry(pos());
    QFontMetrics metrics(textFont);
    int textWidth = qMin(metrics.width(previewText) + horizontalPadding, screenRect.width());
    int textHeight = metrics.height() + verticalPadding;
    setFixedSize(textWidth, textHeight);

    // Background
    painter.fillRect(0, 0, width(), height(), backgroundColor);

    QPen pen;

    // Border
    pen.setColor(borderColor);
    painter.setPen(pen);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // Text
    pen.setColor(textColor);
    painter.setPen(pen);
    painter.setFont(textFont);
    painter.drawText(QRect(0 + horizontalPadding / 2, 0, width(), height()),
                     Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                     previewText);
}

