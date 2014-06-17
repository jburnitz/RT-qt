#include "network.h"
#include "http.h"

#include <QUrlQuery>


network::network(QObject *parent)
{
    conn = new Http();
    connect(conn,SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessData(QByteArray)));
    conn->Get(QUrl("https://helpdesk.uic.edu/las"));
}
network::~network(){
    delete conn;
}
void network::ProcessData(QByteArray data){
    qDebug()<< "Network::ProcessData";

    QString doc(data);
    qDebug() << doc;
    if(doc.contains("UserID")){
        //do login stuff
        qDebug() << "Found Login Page";
        QUrlQuery postData;
        postData.addQueryItem("UserID", "jburni2");
        //postData.addQueryItem("param2", "string");
        conn->Clear();
        conn->Post(QUrl("https://ness.uic.edu/bluestem/login.cgi"),postData);
    }
}
