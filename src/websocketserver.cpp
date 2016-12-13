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

#include <QRegExp>
#include <QUrl>

#include "websocketserver.h"

WebSocketServer::WebSocketServer(Coordinator *coordinator)
    : mCoordinator(coordinator),
      mServer("", QWebSocketServer::NonSecureMode)
{
    connect(&mServer, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
}

bool WebSocketServer::listen()
{
    return mServer.listen(QHostAddress::LocalHost);
}

quint16 WebSocketServer::serverPort() const
{
    return mServer.serverPort();
}

void WebSocketServer::onNewConnection()
{
    QWebSocket *socket = mServer.nextPendingConnection();

    // Extract the room and user ID from the URL
    int roomId, userId;
    QRegExp rx("^/(\\d+)/(\\d+)");
    if (rx.indexIn(socket->requestUrl().path()) != -1) {
        roomId = rx.cap(1).toInt();
        userId = rx.cap(2).toInt();
    } else {
        socket->close();
        return;
    }

    // Add the new client to the coordinator
    mCoordinator->addClient(new Client(socket, userId), roomId);
}
