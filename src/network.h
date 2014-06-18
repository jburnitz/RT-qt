#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QByteArray>

class Http;
class QDomDocument;

class network : public QObject
{
    Q_OBJECT

public:
    network(QObject *parent=0);
    ~network();

public slots:
    void ProcessData(QByteArray data);

private:
    Http *conn;
    QString getValue(QString valueName, QString &source);

signals:


};

#endif // NETWORK_H
