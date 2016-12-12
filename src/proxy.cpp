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

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include "proxy.h"

Proxy::Proxy(quint16 port)
    : mPort(port)
{
}

void Proxy::addSocket(QHttpSocket *socket)
{
    // Reparent the socket
    socket->setParent(this);

    // Create a request to the upstream server
    QUrl url(QString("http://localhost:%d%s").arg(mPort).arg(socket->path()));
    QNetworkRequest request(url);

    // Copy all of the request headers
    for (auto i = socket->headers().constBegin(); i != socket->headers().constEnd(); ++i) {
        request.setRawHeader(i.key(), i.value());
    }

    // Issue the request
    QNetworkReply *reply = mNetworkAccessManager.get(request);
    reply->setParent(this);

    // Copy all of the response headers when they arrive
    connect(reply, &QNetworkReply::metaDataChanged, [socket, reply]() {
        foreach (QByteArray headerName, reply->rawHeaderList()) {
            socket->setHeader(headerName, reply->rawHeader(headerName));
        }
        socket->writeHeaders();
    });

    // TODO: investigate if QIODeviceCopier would work here

    // When data arrives from upstream, send it downstream
    connect(reply, &QNetworkReply::readyRead, [socket, reply]() {
        socket->write(reply->readAll());
    });

    // When data arrives from downstream, send it upstream
    connect(socket, &QHttpSocket::readyRead, [socket, reply]() {
        reply->write(socket->readAll());
    });

    // When either end disconnects, sever everything and tear it down
    auto teardown = [socket, reply]() {
        socket->deleteLater();
        reply->deleteLater();
    };
    connect(reply, &QNetworkReply::finished, teardown);
    connect(socket, &QHttpSocket::aboutToClose, teardown);
}
