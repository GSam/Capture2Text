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

#include "CaptureBox.h"
#include "KeyboardHook.h"
#include "MouseHook.h"


CaptureBox::CaptureBox()
    : QWidget(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::Tool),
      backgroundColor(QColor(255, 0, 0, 64)),
      useBackgroundColor(true),
      borderColor(QColor(255, 0, 0, 255)),
      useBorder(true),
      dragPosition(0, 0),
      captureModeLastPt(-1, -1),
      timerUpdateCaptureMode(this),
      autoCaptureTimer(this),
      moveTimer(this)
{
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(1, 1);

    connect(&timerUpdateCaptureMode, &QTimer::timeout, this, &CaptureBox::updateCaptureMode);
    connect(&autoCaptureTimer, &QTimer::timeout, this, &CaptureBox::autoCaptureTimeout);
    connect(&moveTimer, &QTimer::timeout, this, &CaptureBox::moveTimeout);
    connect(&KeyboardHook::getInstance(), &KeyboardHook::keyPressed, this, &CaptureBox::hotkeyPressed);
    connect(&MouseHook::getInstance(), &MouseHook::buttonPressed, this, &CaptureBox::hotkeyPressed);
}

CaptureBox::~CaptureBox()
{

}

void CaptureBox::startCaptureMode()
{
    MouseHook::getInstance().start();
    KeyboardHook::getInstance().addHotkey(CANCEL, Hotkey("Esc"));
    KeyboardHook::getInstance().addHotkey(CAPTURE, Hotkey("Enter"));
    KeyboardHook::getInstance().addHotkey(LEFT, Hotkey("Left"));
    KeyboardHook::getInstance().addHotkey(RIGHT, Hotkey("Right"));
    KeyboardHook::getInstance().addHotkey(UP, Hotkey("Up"));
    KeyboardHook::getInstance().addHotkey(DOWN, Hotkey("Down"));

    QPoint startPt = QCursor::pos();
    QPoint windowStartPt = startPt;

    captureModePivot = startPt;
    windowStartPt.setX(startPt.x());
    windowStartPt.setY(startPt.y());
    move(windowStartPt);
    setFixedSize(1, 1);
    show();
    raise();
    updateCaptureMode();

    timerUpdateCaptureMode.start(captureModeUpdateMilliSec);
}

void CaptureBox::endCaptureMode()
{
    MouseHook::getInstance().endThread();
    KeyboardHook::getInstance().removeHotkey(CANCEL);
    KeyboardHook::getInstance().removeHotkey(CAPTURE);
    KeyboardHook::getInstance().removeHotkey(LEFT);
    KeyboardHook::getInstance().removeHotkey(RIGHT);
    KeyboardHook::getInstance().removeHotkey(UP);
    KeyboardHook::getInstance().removeHotkey(DOWN);

    moveTimer.stop();
    timerUpdateCaptureMode.stop();
    captureModeLastPt.setX(-1);
    captureModeLastPt.setY(-1);
    hide();
}

void CaptureBox::hotkeyPressed(int id)
{
    if(id == MouseHook::LEFT_MOUSE_DOWN)
    {
        emit captured();
    }
    else if(id == MouseHook::RIGHT_MOUSE_DOWN)
    {
        moveCaptureBoxSize = QSize(width(), height());
    }
    else if(id == CANCEL)
    {
        endCaptureMode();
        emit cancel();
    }
    else if(id == CAPTURE)
    {
        emit captured();
    }
    else if(id == LEFT)
    {
        move(x() - 1, y());
        captureModePivot.setX(x());
        startMoveTimer();
        emit moved();
    }
    else if(id == RIGHT)
    {
        move(x() + 1, y());
        captureModePivot.setX(x());
        startMoveTimer();
        emit moved();
    }
    else if(id == UP)
    {
        move(x(), y() - 1);
        captureModePivot.setY(y());
        startMoveTimer();
        emit moved();
    }
    else if(id == DOWN)
    {
        move(x(), y() + 1);
        captureModePivot.setY(y());
        startMoveTimer();
        emit moved();
    }
}

void CaptureBox::updateCaptureMode()
{
    if(isVisible())
    {
        // Keep above other windows
        raise();
    }

    QPoint ptCursor = QCursor::pos();

    if(isVisible() && (captureModeLastPt != ptCursor))
    {
        startMoveTimer();
        captureModeLastPt = ptCursor;
        int x = ptCursor.x();
        int y = ptCursor.y();
        int x1 = x;
        int y1 = y;
        int x2 = x;
        int y2 = y;

        bool moveBothCorners = MouseHook::getInstance().getRightMouseButtonHeld();

        if(x < captureModePivot.x())
        {
            x1 = x;

            if(moveBothCorners)
            {
                captureModePivot.setX(x1 + moveCaptureBoxSize.width());
            }

            x2 = captureModePivot.x();
        }
        else
        {
            x2 = x;

            if(moveBothCorners)
            {
                captureModePivot.setX(x2 - moveCaptureBoxSize.width());
            }

            x1 = captureModePivot.x();
        }

        if(y < captureModePivot.y())
        {
            y1 = y;

            if(moveBothCorners)
            {
                captureModePivot.setY(y1 + moveCaptureBoxSize.height());
            }

            y2 = captureModePivot.y();
        }
        else
        {
            y2 = y;

            if(moveBothCorners)
            {
                captureModePivot.setY(y2 - moveCaptureBoxSize.height());
            }

            y1 = captureModePivot.y();
        }

        if(x2 - 1 < x1)
        {
            x2 = x1 + 1;
        }

        if(y2 - 1 < y1)
        {
            y2 = y1 + 1;
        }

        setFixedSize(x2 - x1, y2 - y1);
        move(x1, y1);

        emit moved();
    }
}

void CaptureBox::startMoveTimer()
{
    moveTimer.start(moveTimeoutMilliSec);
}

void CaptureBox::autoCapture(QRect &rect)
{
    setFixedSize(rect.width() + 2, rect.height() + 2);
    move(rect.x() - 1, rect.y() - 1);
    show();
    raise();
    autoCaptureTimer.start(500);
}

QRect CaptureBox::getCaptureRect()
{
    QRect captureRect(x() + 1, y() + 1, width() - 2, height() - 2);
    return captureRect;
}

void CaptureBox::autoCaptureTimeout()
{
    autoCaptureTimer.stop();
    hide();
}

void CaptureBox::moveTimeout()
{
    moveTimer.stop();
    emit stoppedMoving();
}

void CaptureBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Draw the capture box rect
    if(useBackgroundColor)
    {
        painter.fillRect(0, 0, width(), height(), backgroundColor);
    }

    if(useBorder)
    {
        // Draw border around capture box rect
        QPen pen;
        pen.setColor(borderColor);
        painter.setPen(pen);
        painter.drawRect(0, 0, width() - 1, height() - 1);
    }
}
