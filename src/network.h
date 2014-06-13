#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QNetworkRequest>

class QNetworkReply;
class QNetworkAccessManager;
class QUrl;
class QByteArray;
class QNetworkCookieJar;

class network : public QObject
{
    Q_OBJECT

public:
    network(QObject *parent);
    ~network();

    void SetUrl(QUrl url);
    QByteArray GetData();
    void Get();
    void Post(QUrl params);

private:
    QObject *parent;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *theReply;
    QUrl redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const;
    QUrl _urlRedirectedTo;

signals:
    void data(QNetworkReply *theData);

private slots:
    void replyFinished(QNetworkReply *reply);
};

#endif // NETWORK_H
