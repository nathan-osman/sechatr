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
#include <QObject>

#include "coordinator.h"
#include "httpserver.h"
#include "pollmanager.h"
#include "websocketserver.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Coordinator coordinator;

    // Create options for the application
    QCommandLineOption addrOption("addr", "address for HTTP server", "address", "0.0.0.0");
    QCommandLineOption portOption("port", "port for HTTP server", "port", "8000");
    QCommandLineOption directoryOption("directory", "directory for storage", "directory", ".");

    // Create the command-line parser and add the options
    QCommandLineParser parser;
    parser.addOption(addrOption);
    parser.addOption(portOption);
    parser.addOption(directoryOption);
    parser.addHelpOption();

    // Parse the options
    if (!parser.parse(app.arguments())) {
        app.exit(1);
    }

    // Process the help option if used
    if (parser.isSet("help")) {
        parser.showHelp();
    }

    // Create the poll manager
    PollManager pollManager(parser.value(directoryOption));

    // Create the local WebSocket server
    WebSocketServer webSocketServer(&coordinator);
    if (!webSocketServer.listen()) {
        qCritical("Unable to find an empty port");
        return 1;
    }

    // Create the HTTP server
    HttpServer httpServer(&coordinator, webSocketServer.serverPort());
    if (!httpServer.listen(QHostAddress(parser.value(addrOption)), parser.value(portOption).toInt())) {
        qCritical("Unable to listen on port %d", parser.value(portOption).toInt());
        return 1;
    }

    return app.exec();
}
