#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QByteArray>

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
    void Load();
    QString doc;

public slots:
    void ProcessData(QByteArray data);
    void ProcessREST(QByteArray data);

protected:
    Http *conn;
    QString id,password;
    QString getValue(QString valueName, QString &source);
    QString getDataBetween(QString begin,QString end, QString &source);
    void Login();

    ticket* GetQueues();
    //GetTickets(QString queue)
    ticket* queues;

signals:
    void CredentialsRequested();
    void LoggedIn();

};

#endif // NETWORK_H
