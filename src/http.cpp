/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#include <QtWidgets>
#include <QtNetwork>
#include "http.h"

Http::Http(QObject *parent)
    : QObject(parent)
{
    //default constructor
}

void Http::startRequestGet(QUrl url)
{
    reply = qnam.get(QNetworkRequest(url));

    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    //could be useful someday
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void Http::startRequestPost(QByteArray params){
    reply = qnam.post(QNetworkRequest(url), params);
}

void Http::Post(QUrl theUrl, QByteArray params){
    this->url = url;

    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();
    if (fileName.isEmpty())
        fileName = "index.post.html";

    file = new QFile(fileName);

    startRequestPost(params);
}

void Http::downloadFile(QUrl theUrl)
{
    url = theUrl;

    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();
    if (fileName.isEmpty())
        fileName = "index.html";

    file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        delete file;
        file = 0;
        return;
    }

    // schedule the request
    httpRequestAborted = false;
    //Get the data
    startRequestGet(url);
}
void Http::httpFinished()
{
    qDebug()<< "Http::httpFinished";
    file->flush();
    file->close();
    emit(httpDone(data));
    //qDebug() << data;

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        qWarning() << "HTTP Download failed: " << reply->errorString();

    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        url = newUrl;
        reply->deleteLater();
        file->open(QIODevice::WriteOnly);
        file->resize(0);
        startRequest(url);
        return;
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
}

void Http::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file){
        //file->write(reply->readAll());
        data.append(reply->readAll());
        qDebug() << data.size();
    }
}
