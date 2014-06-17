#include "network.h"
#include "http.h"

#include <QRegExp>


network::network(QObject *parent)
{
    conn = new Http();
    connect(conn,SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessData(QByteArray)));
    conn->downloadFile(QUrl("https://helpdesk.uic.edu/las"));
}
network::~network(){
    delete conn;
}
void network::ProcessData(QByteArray data){
    //qDebug()<< "Network::ProcessData";
    //qDebug() << data;
    //QRegExp titleBegin ("<title>", Qt::CaseInsensitive);
   // QRegExp titleEnd ("Found", Qt::CaseInsensitive);
    //titleBegin.setMinimal(true);
    //titleEnd.setMinimal(true);
    QString doc(data);
    if(doc.contains("UserID"))
        qDebug() << "donzo";

}
