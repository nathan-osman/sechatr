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
#include <QRegExp>

#include "websocketserver.h"

WebSocketServer::WebSocketServer()
    : mServer("", QWebSocketServer::NonSecureMode)
{
    connect(&mServer, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
}

WebSocketServer::~WebSocketServer()
{
    // TODO: graceful shutdown

    // Delete the clients in each room and finally the room
    foreach (int roomId, mClients.keys()) {
        foreach (Client *client, *mClients.value(roomId)) {
            client->socket->deleteLater();
            delete client;
        }
        delete mClients.value(roomId);
    }
}

bool WebSocketServer::listen(const QHostAddress &address, quint16 port)
{
    return mServer.listen(address, port);
}

void WebSocketServer::onNewConnection()
{
    QWebSocket *socket = mServer.nextPendingConnection();

    // Make sure the client is using a valid URL; otherwise kick them
    int roomId, userId;
    if (!parseRequestUrl(socket->requestUrl(), roomId, userId)) {
        socket->close();
    }

    // Build a new client for the new connection
    Client *client = new Client{
        socket,
        roomId,
        userId,
        false,  // active?
        0,      // last message read
        0       // last char entered
    };

    // Invoke the appropriate action when signals are emitted
    connect(socket, &QWebSocket::textMessageReceived, [this, client](const QString &message) {
        processMessage(client, message);
    });
    connect(socket, &QWebSocket::disconnected, [this, client]() {
        processDisconnect(client);
    });

    // If the room is empty, add the new client; otherwise, broadcast data for
    // everyone else in the room
    if (!mClients.contains(roomId)) {
        mClients.insert(roomId, new ClientList);
    } else {
        foreach (Client *peer, *mClients.value(roomId)) {
            peer->socket->sendTextMessage(createPacket("active", peer->userId, peer->active));
            peer->socket->sendTextMessage(createPacket("position", peer->userId, peer->lastMessageRead));
            peer->socket->sendTextMessage(createPacket("typing", peer->userId, peer->lastCharEntered));
        }
    }

    // Add the new client
    mClients.value(roomId)->append(client);
}

QByteArray WebSocketServer::createPacket(const QString &type, int userId, int value)
{
    QJsonObject object{
        {"type", type},
        {"user_id", userId},
        {"value", value}
    };
    return QJsonDocument(object).toJson();
}

bool WebSocketServer::parseRequestUrl(const QUrl &url, int &roomId, int &userId)
{
    QRegExp rx("^/(\\d+)/(\\d+)");
    if (rx.indexIn(url.path()) != -1) {
        roomId = rx.cap(1).toInt();
        userId = rx.cap(2).toInt();
        return true;
    } else {
        return false;
    }
}

void WebSocketServer::processMessage(Client *client, const QString &message)
{
    // Message is a UTF-8 encoded JSON document
    QJsonObject object = QJsonDocument::fromJson(message.toUtf8()).object();
    QString type = object.value("type").toString();

    // For a ping, send an empty JSON object in reply
    if (type == "ping") {
        client->socket->sendTextMessage("{}");
        return;
    }

    // For every other message, update client state
    int value = object.value("value").toInt();
    if (type == "active") {
        client->active = static_cast<bool>(value);
    } else if (type == "position") {
        client->lastMessageRead = value;
    } else if (type == "typing") {
        client->lastCharEntered = value;
    }

    // Broadcast the packet to the other peers in the room
    foreach (Client *peer, *mClients.value(client->roomId)) {
        if (peer != client) {
            peer->socket->sendTextMessage(message);
        }
    }
}

void WebSocketServer::processDisconnect(Client *client)
{
    // Remove the user from their room
    mClients.value(client->roomId)->removeAll(client);

    // Remove the room if it is empty
    if (!mClients.value(client->roomId)->count()) {
        delete mClients.take(client->roomId);
    }
    // Broadcast it to the room
    QByteArray message = createPacket("quit", client->userId);
    foreach (Client *peer, *mClients.value(client->roomId)) {
        peer->socket->sendTextMessage(message);
    }

    // Free everything up
    client->socket->deleteLater();
    delete client;
}
