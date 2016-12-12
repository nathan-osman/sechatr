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

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include "coordinator.h"
#include "httpserver.h"
#include "websocketserver.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Coordinator coordinator;

    // Create options for the application
    QCommandLineOption httpAddrOption("http-addr", "address for HTTP server", "address", "0.0.0.0");
    QCommandLineOption httpPortOption("http-port", "port for HTTP server", "port", "8000");
    QCommandLineOption webSocketAddrOption("websocket-addr", "address for websocket server", "address", "0.0.0.0");
    QCommandLineOption webSocketPortOption("websocket-port", "port for websocket server", "port", "8001");

    // Create the command-line parser and add the options
    QCommandLineParser parser;
    parser.addOption(httpAddrOption);
    parser.addOption(httpPortOption);
    parser.addOption(webSocketAddrOption);
    parser.addOption(webSocketPortOption);
    parser.addHelpOption();

    // Parse the options
    if (!parser.parse(app.arguments())) {
        app.exit(1);
    }

    // Process the help option if used
    if (parser.isSet("help")) {
        parser.showHelp();
    }

    HttpServer httpServer(&coordinator);
    if (!httpServer.listen(QHostAddress(parser.value(httpAddrOption)), parser.value(httpPortOption).toInt())) {
        qCritical("Unable to listen on port %d", parser.value(httpPortOption).toInt());
        return 1;
    }

    WebSocketServer webSocketServer(&coordinator);
    if (!webSocketServer.listen(QHostAddress(parser.value(webSocketAddrOption)), parser.value(webSocketPortOption).toInt())) {
        qCritical("Unable to listen on port %d", parser.value(webSocketPortOption).toInt());
        return 1;
    }

    return app.exec();
}
