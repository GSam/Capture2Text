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

#ifndef REPLYTIMEOUT_H
#define REPLYTIMEOUT_H

#include <QtNetwork/QNetworkReply>
#include <QTimerEvent>
#include <QBasicTimer>

// https://codereview.stackexchange.com/questions/30031/qnetworkreply-network-reply-timeout-helper
class ReplyTimeout : public QObject
{
    Q_OBJECT
    QBasicTimer m_timer;
public:
    ReplyTimeout(QNetworkReply* reply, const int timeout) : QObject(reply)
    {
        Q_ASSERT(reply);

        if (reply && reply->isRunning())
        {
            m_timer.start(timeout, this);
        }
    }
    static void set(QNetworkReply* reply, const int timeout)
    {
        new ReplyTimeout(reply, timeout);
    }
protected:
    void timerEvent(QTimerEvent * ev)
    {
        if (!m_timer.isActive() || ev->timerId() != m_timer.timerId())
        {
            return;
        }

        auto reply = static_cast<QNetworkReply*>(parent());

        if (reply->isRunning())
        {
            reply->close();
        }

        m_timer.stop();
    }
};

#endif // REPLYTIMEOUT_H
