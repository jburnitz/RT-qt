#include "network.h"
#include "http.h"

#include <QUrlQuery>

#include <QDomDocument>
#include <QXmlInputSource>

network::network(QObject *parent)
{
    this->setParent(parent);

    conn = new Http();
    connect(conn,SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessData(QByteArray)));
}
network::~network(){
    delete conn;
}

void network::Begin(){
    conn->Get(QUrl("https://helpdesk.uic.edu/las"));
}
void network::Fetch(QString url){
    conn->Clear();
    conn->Get(QUrl(url));
}

void network::SetCredentials(QString id, QString password){
    this->id = id;
    this->password = password;

    Login();
}

void network::Login(){
    qDebug()<<"network::Login";

    QString doc(conn->data);

    //building the login url
    QUrlQuery postData;
    postData.addQueryItem("UserID", id);
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

    if( doc.contains("passwordInput")){
        postData.addQueryItem("password", password);
    }
    conn->Clear();
    conn->Post(QUrl("https://ness.uic.edu/bluestem/login.cgi"),postData);
}

void network::ProcessData(QByteArray data){
    qDebug()<< "Network::ProcessData";

    doc.clear();
    doc.append(data);
    if(doc.contains("UserIDinput")){
        //prompt for id and pass
        emit(CredentialsRequested());
    }else if( doc.contains("passwordInput")){
        Login();
    }else if( doc.contains("retrieve_auth.cgi")){
        conn->Clear();
        conn->Get(QUrl(getDataBetween("<a href=\"","\">here", doc)));
    }else{
        //qDebug() << doc;
        qDebug()<<"emit(LoggedIn())";
        disconnect(conn, SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessData(QByteArray)));
        connect(conn, SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessREST(QByteArray)));
        emit(LoggedIn());
    }

}

void network::ProcessREST(QByteArray data){
    qDebug()<< "network::ProcessREST()" << this->doc;
    QDomDocument document;
    QXmlInputSource is;
    is.setData(this->doc);
    document.setContent(is.data());

    QDomNodeList nodes = document.elementsByTagName("Subject");
    if( nodes.size() > 0)
        qDebug() << nodes.at(0).toElement().text();

}

QString network::getValue(QString valueName, QString &source){
    valueName.append("\" value=\"");
    int startIndex = source.indexOf(valueName)+valueName.length();
    if(startIndex <= 0)return QString();
    int endIndex = source.indexOf("\"/>",startIndex);
    if(endIndex <= 0)return QString();
    return source.mid(startIndex,endIndex - startIndex);
}

QString network::getDataBetween(QString begin,QString end, QString &source){
    int startIndex = source.indexOf(begin)+begin.length();
    if(startIndex <= 0)return QString();
    int endIndex = source.indexOf(end,startIndex);
    if(endIndex <= 0)return QString();
    return source.mid(startIndex,endIndex - startIndex);
}
