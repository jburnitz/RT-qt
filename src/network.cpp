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
    if(doc.contains("UserIDinput")){
        //do login stuff
        //prompt for id and pass
        qDebug() << "Found Login Page, building reply";
        QUrlQuery postData;
        postData.addQueryItem("UserID", "jburni2");

        postData.addQueryItem("RPCVersion",getValue("RPCVersion", doc));
        postData.addQueryItem("RPCURL",getValue("RPCURL", doc));
        postData.addQueryItem("return",getValue("return", doc));
        postData.addQueryItem("RetrieveURL",getValue("RetrieveURL", doc));
        postData.addQueryItem("proxyrpc",getValue("proxyrpc", doc));
        postData.addQueryItem("proxy",getValue("proxy", doc));
        postData.addQueryItem("prior",getValue("prior", doc));
        postData.addQueryItem("msg",getValue("msg", doc));
        postData.addQueryItem("cacheid",getValue("cacheid", doc));
        postData.addQueryItem("BSVersionHash",getValue("BSVersionHash", doc));
        postData.addQueryItem("BSVersion",getValue("BSVersion", doc));

        conn->Clear();
        conn->Post(QUrl("https://ness.uic.edu/bluestem/login.cgi"),postData);
    }
}

QString network::getValue(QString valueName, QString &source){
    valueName.append("\" value=\"");
    int startIndex = source.indexOf(valueName)+valueName.length();
    if(startIndex <= 0)return QString();
    int endIndex = source.indexOf("\"\/>",startIndex);
    if(endIndex <= 0)return QString();
    return source.mid(startIndex,endIndex - startIndex);
}
