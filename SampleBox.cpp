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

#include <QPainter>
#include <QFontMetrics>
#include "SampleBox.h"

SampleBox::SampleBox(QWidget *parent)
    : QWidget(parent),
      previewText("Sample Preview Text"),
      textColor(QColor(128, 128, 128, 255)),
      backgroundColor(QColor(255, 0, 0 , 100)),
      borderColor(QColor(255, 0, 0, 255)),
      textFont(QFont("Arial", 16)),
      isCaptureBoxSample(true)
{

}

void SampleBox::drawCheckerboard(QPainter &painter)
{
    const int squareSize = 20;

    // Draw checkerboard
    bool isWhite = false;
    int numSquaresX = width() / squareSize + 1;
    int numSquaresY = height() / squareSize + 1;

    for(int y = 0; y < numSquaresY; y++)
    {
        isWhite = ((y % 2) == 0);

        for(int x = 0; x < numSquaresX; x++)
        {
            QColor color(200, 200, 200, 255);

            if(isWhite)
            {
                color.setRgb(255, 255, 255, 255);
            }

            painter.fillRect(x * squareSize, y * squareSize,
                             squareSize, squareSize, color);

            isWhite = !isWhite;
        }
    }

    // Draw border
    QColor borderColor(128, 128, 128, 255);
    QPen pen;
    pen.setColor(borderColor);
    painter.setPen(pen);
    painter.drawRect(0, 0, width() - 1, height() - 1);
}

void SampleBox::paintEvent(QPaintEvent *)
{
    const float gapPercent = 0.10f;
    QPainter painter(this);
    drawCheckerboard(painter);

    if(isCaptureBoxSample)
    {
        // Background
        painter.fillRect(width() * gapPercent + 1,
                         height() * gapPercent + 1,
                         width() * (1.0f - gapPercent * 2) - 2,
                         height() * (1.0f - gapPercent * 2) - 2, backgroundColor);

        // Border
        QPen pen;
        pen.setColor(borderColor);
        painter.setPen(pen);
        painter.drawRect(width() * gapPercent,
                         height() * gapPercent,
                         width() * (1.0f - gapPercent * 2) - 1,
                         height() * (1.0f - gapPercent * 2) - 1);
    }
    else
    {
        QFontMetrics metrics(textFont);
        int textWidth = qMin(metrics.width(previewText) + horizontalPadding, width());
        int textHeight = metrics.height() + verticalPadding;

        // Background
        painter.fillRect(width() * gapPercent + 1,
                         height() * gapPercent + 1,
                         textWidth,
                         textHeight,
                         backgroundColor);

        // Border
        QPen pen;
        pen.setColor(borderColor);
        painter.setPen(pen);
        painter.drawRect(width() * gapPercent,
                         height() * gapPercent,
                         textWidth + 1,
                         textHeight + 1);

        // Text
        pen.setColor(textColor);
        painter.setPen(pen);
        painter.setFont(textFont);
        painter.drawText(QRect(width() * gapPercent + horizontalPadding / 2,
                               height() * gapPercent,
                               textWidth,
                               textHeight),
                         Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                         previewText);
    }
}
