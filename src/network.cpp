#include "network.h"

#include <QtNetwork>

network::network()
{
    finished = false;
    manager = new QNetworkAccessManager(this);
    manager->setCookieJar( new QNetworkCookieJar(this) );
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
}

network::~network(){
    delete manager;
}

void network::SetUrl(QUrl url){
    request = QNetworkRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
}

void network::replyFinished(QNetworkReply *reply){
   QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

   QList<QNetworkCookie>  cookies = manager->cookieJar()->cookiesForUrl(reply->url());
   qDebug() << "COOKIES for" << reply->url().host() << cookies;

   if(!possibleRedirectUrl.toUrl().isEmpty()){
       qDebug() << "redirect to ->" << possibleRedirectUrl.toUrl().toDisplayString();
       request.setUrl( possibleRedirectUrl.toUrl() );
       Get();
   }else{
       finished = true;
       theReply = reply;
   }

   QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

       /* We'll deduct if the redirection is valid in the redirectUrl function */
       _urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(),_urlRedirectedTo);

       /* If the URL is not empty, we're being redirected. */
       if(!_urlRedirectedTo.isEmpty()) {
           QString text = QString("QNAMRedirect::replyFinished: Redirected to ").append(_urlRedirectedTo.toString());
           this->_textContainer->setText(text);
           /* We'll do another request to the redirection url. */
           this->_qnam->get(QNetworkRequest(_urlRedirectedTo));
       }
       else {
           /*
            * We weren't redirected anymore
            * so we arrived to the final destination...
            */
           QString text = QString("QNAMRedirect::replyFinished: Arrived to ")
                                 .append(reply->url().toString());
           this->_textContainer->setText(text);
           /* ...so this can be cleared. */
           _urlRedirectedTo.clear();
       }
}

void network::Get(){
    manager->get(request);
}
void network::Post(QUrl params){
    manager->post(request, params.toEncoded());
}

QByteArray network::GetData(){
    //qDebug() << cookieJar->allCookies();
    if(finished)
        return theReply->readAll();
    else
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
