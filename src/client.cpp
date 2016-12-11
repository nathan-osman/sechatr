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

#include <QJsonDocument>
#include <QJsonObject>

#include "client.h"

Client::Client(QWebSocket *socket, int userId)
    : mSocket(socket),
      mUserId(userId),
      mActive(false),
      mLastMessageRead(0),
      mLastCharEntered(0)
{
    connect(mSocket, &QWebSocket::textMessageReceived, this, &Client::onTextMessageReceived);
    connect(mSocket, &QWebSocket::disconnected, this, &Client::disconnected);
}

Client::~Client()
{
    mSocket->deleteLater();
}

void Client::sendMessage(Type type, int value)
{
    QJsonObject object{
        {"type", typeToString(type)},
        {"user_id", mUserId},
        {"value", value}
    };
    mSocket->sendTextMessage(QJsonDocument(object).toJson());
}

int Client::userId() const
{
    return mUserId;
}

bool Client::isActive() const
{
    return mActive;
}

int Client::lastMessageRead() const
{
    return mLastMessageRead;
}

int Client::lastCharEntered() const
{
    return mLastCharEntered;
}

void Client::onTextMessageReceived(const QString &message)
{
    QJsonObject object = QJsonDocument::fromJson(message.toUtf8()).object();
    Type type = stringToType(object.value("type").toString());
    int value = object.value("value").toInt();

    switch (type) {
    case Ping:
        sendMessage(Pong);
        return;
    case Active:
        mActive = static_cast<bool>(value);
        break;
    case Position:
        mLastMessageRead = value;
        break;
    case Typing:
        mLastCharEntered = value;
        break;
    }

    emit messageReceived(type, value);
}

QString Client::typeToString(Type type) const
{
    switch (type) {
    case Active:
        return "active";
    case Pong:
        return "pong";
    case Position:
        return "position";
    case Quit:
        return "quit";
    case Typing:
        return "typing";
    default:
        return "";
    }
}

Client::Type Client::stringToType(const QString &string) const
{
    if (string == "active") {
        return Active;
    } else if (string == "ping") {
        return Ping;
    } else if (string == "position") {
        return Position;
    } else if (string == "typing") {
        return Typing;
    }
}
