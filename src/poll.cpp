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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>

#include "poll.h"

void Poll::setTitle(const QString &title)
{
    mTitle = title;
}

void Poll::addOption(const QString &label)
{
    mOptions.append(label);
}

void Poll::addVote(int userId, int selection)
{
    mVotes.insert(QString::number(userId), selection);
}

QByteArray Poll::serialize() const
{
    return QJsonDocument(QJsonObject{
        { "title", mTitle },
        { "options", QJsonValue::fromVariant(mOptions) },
        { "votes", QJsonValue::fromVariant(mVotes) }
    }).toJson();
}

void Poll::deserialize(const QByteArray &data)
{
    QJsonObject object = QJsonDocument::fromJson(data).object();
    mTitle = object.value("title").toString();
    mOptions = object.value("options").toVariant().toStringList();
    mVotes = object.value("votes").toVariant().toMap();
}
