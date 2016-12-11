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

#include "room.h"

Room::Room(int roomId)
    : mRoomId(roomId)
{
}

Room::~Room()
{
    qDeleteAll(*this);
}

void Room::addClient(Client *client)
{
    // Send the state of other clients in the room to the new client
    for (auto i = constBegin(); i != constEnd(); ++i) {
        client->sendMessage(Client::Active, (*i)->userId(), (*i)->isActive());
        client->sendMessage(Client::Position, (*i)->userId(), (*i)->lastMessageRead());
        client->sendMessage(Client::Typing, (*i)->userId(), (*i)->lastCharEntered());
    }

    // Add the new client to the room
    append(client);
}

void Room::onMessageReceived(Client::Type type, int value)
{
    Client *client = qobject_cast<Client*>(sender());

    // Broadcast the message to the rest of the room
    for (auto i = constBegin(); i != constEnd(); ++i) {
        if (*i != client) {
            (*i)->sendMessage(type, value);
        }
    }
}

void Room::onDisconnected()
{
    Client *client = qobject_cast<Client*>(sender());

    // Remove the client from the room
    removeAll(client);

    // Broadcast the removal to the other peers in the room
    for (auto i = constBegin(); i != constEnd(); ++i) {
        (*i)->sendMessage(Client::Quit, client->userId());
    }

    // Free the client
    delete client;

    // If the room is empty, emit the signal
    if (!count()) {
        emit roomEmptied();
    }
}
