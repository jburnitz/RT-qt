#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QByteArray>

class Http;

class network : public QObject
{
    Q_OBJECT

public:
    network(QObject *parent=0);
    ~network();
    void SetCredentials(QString id, QString password);
    void Begin();
    QString doc;

public slots:
    void ProcessData(QByteArray data);

private:
    Http *conn;
    QString id,password;
    QString getValue(QString valueName, QString &source);
    QString getDataBetween(QString begin,QString end, QString &source);
    void Login();

signals:
    void CredentialsRequested();
    void LoggedIn();

};

#endif // NETWORK_H
