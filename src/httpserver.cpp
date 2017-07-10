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
#include <QSet>

#include "httpserver.h"

HttpServer::HttpServer(Coordinator *coordinator, quint16 webSocketPort)
    : mCoordinator(coordinator),
      mWebSocketProxy(QHostAddress::LocalHost, webSocketPort)
{
    mFilesystemHandler.setDocumentRoot(":");
    mFilesystemHandler.addRedirect(QRegExp("^$"), "/static/index.html");
    mFilesystemHandler.addSubHandler(QRegExp("^api/"), &mApiHandler);
    mFilesystemHandler.addSubHandler(QRegExp("^ws/"), &mWebSocketProxy);

    // TODO: legacy redirect; remove after a while
    mFilesystemHandler.addRedirect(QRegExp("^js/chatstatus.js"), "/static/js/chatstatus.js");

    mApiHandler.registerMethod("stats", this, &HttpServer::onStats);

    mServer.setHandler(&mFilesystemHandler);
}

bool HttpServer::listen(const QHostAddress &address, quint16 port)
{
    return mServer.listen(address, port);
}

void HttpServer::onStats(QHttpEngine::Socket *socket)
{
    QSet<int> userIds;

    for (auto i = mCoordinator->constBegin(); i != mCoordinator->constEnd(); ++i) {
        for (auto j = (*i)->constBegin(); j < (*i)->constEnd(); ++j) {
            userIds.insert((*j)->userId());
        }
    }

    QJsonObject object{
        {"num_rooms", mCoordinator->count()},
        {"num_users", userIds.count()},
    };

    socket->writeJson(QJsonDocument(object));
}
