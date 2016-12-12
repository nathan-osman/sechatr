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

#ifndef PROXY_H
#define PROXY_H

#include <QNetworkAccessManager>
#include <QObject>

#include <QHttpEngine/QHttpSocket>

/**
 * @brief Seamless proxy for websocket connections
 *
 * The WebSocket server must listen on a separate port (Qt limitation) so we
 * can work around this issue by proxying the websocket connections until they
 * are closed. This is as simple as copying reads and writes back and forth
 * once the connection is established.
 */
class Proxy : public QObject
{
    Q_OBJECT

public:

    explicit Proxy(quint16 port);

    void addSocket(QHttpSocket *socket);

private:

    quint16 mPort;
    QNetworkAccessManager mNetworkAccessManager;
};

#endif // PROXY_H
