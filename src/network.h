#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QUrl>

class Http;
class ticket;

class network : public QObject
{
    Q_OBJECT

public:
    network(QObject *parent=0);
    ~network();
    void SetCredentials(QString id, QString password);
    void Begin();
    void Fetch(QString url);
    void Fetch(QUrl url);
    void Load();
    QString doc;
    QUrl baseUrl;

public slots:
    void ProcessData(QByteArray data);
    void ProcessREST(QByteArray data);
    void FindQueues(QByteArray data);
    void ShowQueues();

protected:
    Http *conn;
    QString id,password;
    QString getValue(QString valueName, QString &source);
    QString getDataBetween(QString begin,QString end, QString &source);
    void Login();

    QMap<QString, QString> queues;
    int qId;

signals:
    void CredentialsRequested();
    void LoggedIn();
    void Error(const QString err, const QString details);
    void Done();

};

#endif // NETWORK_H
