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
    this->httpGetRequest = true;
}

void Http::startRequest()
{
    if(httpGetRequest){
        qDebug()<<"GET with url=" << url.toString();
        reply = qnam.get(QNetworkRequest(url));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(netError()));
        connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    }
    else{
        qDebug()<<"POST with url=" << url.toString() << " param=" << params.query();
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        replyPost = qnam.post( request, params.toString(QUrl::FullyEncoded).toUtf8());
        connect(replyPost, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(netError()));
        connect(replyPost, SIGNAL(finished()), this, SLOT(httpFinished()));
        connect(replyPost, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    }
    //could be useful someday
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
}
void Http::netError(){

    qDebug()<<"error!";
}

void Http::Post(QUrl theUrl, QUrlQuery params){
    this->url = theUrl;
    this->params = params;
    this->httpGetRequest = false;

    startRequest();
}
void Http::Post(QUrlQuery params){
    this->Post(this->url, params);
}

void Http::Get(QUrl theUrl)
{
    httpGetRequest = true;
    url = theUrl;

    startRequest();
}
void Http::httpFinished()
{
    qDebug()<< "Http::httpFinished";

    if(httpGetRequest){
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        qWarning() << "HTTP Download failed: " << reply->errorString();
        return;

    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        url = newUrl;
        reply->deleteLater();
        startRequest();
        return;
    }
    reply->deleteLater();
    reply = 0;
    }
    else { //handle the POST
        QVariant redirectionTarget = replyPost->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (replyPost->error()) {
            qWarning() << "HTTP POST failed: " << replyPost->errorString();

        } else if (!redirectionTarget.isNull()) {
            QUrl newUrl = url.resolved(redirectionTarget.toUrl());
            url = newUrl;
            replyPost->deleteLater();
            startRequest();
            return;
        }
        replyPost->deleteLater();
        replyPost = 0;
    }

    qDebug()<< "Http::httpFinished emit httpDone(qbytearray data)";
    //qDebug()<<data;
    emit(httpDone(data));
}

void Http::Clear(){
    qDebug()<<"Http::Clear()";
    data.clear();
    url.clear();
    params.clear();
    httpGetRequest = true;
    httpGetId =0;
}

void Http::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    if(httpGetRequest)
        data.append(reply->readAll());
    else
        data.append(replyPost->readAll());
}
