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

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QUuid>
#include <QVariant>

#include "poll.h"

Poll::Poll(const QString &directory, const QString &title, const QStringList &options)
    : mInitialized(true),
      mUuid(QUuid::createUuid().toString()),
      mFilename(QDir(directory).absoluteFilePath(mUuid)),
      mTitle(title),
      mOptions(options)
{
}

Poll::Poll(const QString &directory, const QString &uuid)
    : mInitialized(false),
      mUuid(uuid),
      mFilename(QDir(directory).absoluteFilePath(mUuid))
{
    QFile file(mFilename);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        return;
    }
    QJsonObject object = document.object();
    mTitle   = object.value("title").toString();
    mOptions = object.value("options").toVariant().toStringList();
    mVotes   = object.value("votes").toVariant().toMap();
    mInitialized = true;
}

Poll::~Poll()
{
    if (mInitialized) {
        serialize();
    }
}

bool Poll::isInitialized() const
{
    return mInitialized;
}

QString Poll::uuid() const
{
    return mUuid;
}

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

void Poll::serialize() const
{
    QFile file(mFilename);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    file.write(QJsonDocument(QJsonObject{
        { "title", mTitle },
        { "options", QJsonValue::fromVariant(mOptions) },
        { "votes", QJsonValue::fromVariant(mVotes) }
    }).toJson());
}
