#include "network.h"

#include <QtNetwork>

network::network(QObject *parent)
{
    this->parent = parent;
    manager = new QNetworkAccessManager(this);
    manager->setCookieJar( new QNetworkCookieJar(this) );
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
    QObject::connect( this, SIGNAL(data(QNetworkReply*)), this->parent, SLOT(ProcessData(QNetworkReply *)));
}

network::~network(){
    delete manager;
    delete theReply;
}

void network::SetUrl(QUrl url){
    request = QNetworkRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
}

void network::replyFinished(QNetworkReply *reply){
    QList<QNetworkCookie>  cookies = manager->cookieJar()->cookiesForUrl(reply->url());
    qDebug() << "COOKIES for " << reply->url().host() << cookies;

    /* We'll deduct if the redirection is valid in the redirectUrl function */
    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    _urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(),_urlRedirectedTo);

    /* If the URL is not empty, we're being redirected. */
    if(!_urlRedirectedTo.isEmpty()) {
        qDebug()<< QString("network::replyFinished: Redirected to ").append(_urlRedirectedTo.toString());
        /* We'll do another request to the redirection url. */
        this->manager->get(QNetworkRequest(_urlRedirectedTo));
    }
    else { //not being redirected anymore.
        qDebug() << QString("network::replyFinished: Arrived to ").append(reply->url().toString());
        _urlRedirectedTo.clear();
        theReply = reply;

        //emit that data
        emit data(theReply);
    }
}
void network::Get(){
    manager->get(request);
}
void network::Post(QUrl params){
    manager->post(request, params.toEncoded());
}

QByteArray network::GetData(){
    return NULL;
}

QUrl network::redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const {
    QUrl redirectUrl;
    /*
     * Check if the URL is empty and
     * that we aren't being fooled into a infinite redirect loop.
     * We could also keep track of how many redirects we have been to
     * and set a limit to it, but we'll leave that to you.
     */
    if(!possibleRedirectUrl.isEmpty() &&
       possibleRedirectUrl != oldRedirectUrl) {
        redirectUrl = possibleRedirectUrl;
    }
    return redirectUrl;
}
