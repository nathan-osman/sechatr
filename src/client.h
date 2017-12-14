/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QVariant>
#include <QWebSocket>

/**
 * @brief Remote client connected via websocket
 */
class Client : public QObject
{
    Q_OBJECT

public:

    enum Type {
        Unknown,
        Quit,
        Ping,
        Pong,
        Message,
        Active,
        Position,
        Typing,
        Vote
    };

    Client(QWebSocket *socket, int userId);
    virtual ~Client();

    void sendMessage(Type type, int userId = 0, const QVariant &value = QVariant());

    int userId() const;
    bool isActive() const;
    int lastMessageRead() const;
    int lastCharEntered() const;

signals:

    void messageReceived(Type type, const QVariant &value);
    void disconnected();

private slots:

    void onTextMessageReceived(const QString &message);

private:

    QString typeToString(Type type) const;
    Type stringToType(const QString &string) const;

    QWebSocket *mSocket;

    int mUserId;
    bool mActive;
    int mLastMessageRead;
    int mLastCharEntered;
};

#endif // CLIENT_H
