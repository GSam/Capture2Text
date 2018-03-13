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

#ifndef CAPTURE_BOX_H
#define CAPTURE_BOX_H

#include <QtGui>
#include <QDialog>
#include <QTimer>


class CaptureBox : public QWidget
{
    Q_OBJECT
public:
    CaptureBox();
    ~CaptureBox();

    void startCaptureMode();
    void endCaptureMode();
    void updateCaptureMode();

    void autoCapture(QRect &rect);

    QRect getCaptureRect();

    void setBackgroundColor(QColor &value) { backgroundColor = value; }
    void setUseBackgroundColor(bool value) { useBackgroundColor = value; }

    bool getUseBorder() const  { return useBorder; }
    void setUseBorder(bool value) { useBorder = value; }

    QColor getBorderColor() const { return borderColor; }
    void setBorderColor(const QColor &value) { borderColor = value; }

public slots:
    void turnOnBackground() { useBackgroundColor = true; repaint(); }
    void turnOffBackground() { useBackgroundColor = false; repaint(); }

signals:
    void captured();
    void moved();
    void stoppedMoving();
    void cancel();

protected:
    void paintEvent(QPaintEvent *event);

private:
    enum HotkeyAction
    {
        CANCEL = 100,
        CAPTURE,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    static const int captureModeUpdateMilliSec = 10;
    static const int moveTimeoutMilliSec = 400;

    void autoCaptureTimeout();
    void moveTimeout();
    void startMoveTimer();
    void hotkeyPressed(int id);

    QTimer timerUpdateCaptureMode;

    QPoint captureModePivot;
    QSize moveCaptureBoxSize;
    QPoint captureModeLastPt;

    bool useBackgroundColor;
    QColor backgroundColor;

    bool useBorder;
    QColor borderColor;

    QTimer autoCaptureTimer;
    QPoint dragPosition;

    QTimer moveTimer;
};

#endif // CAPTURE_BOX_H
