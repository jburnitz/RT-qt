#include "network.h"
#include "http.h"
#include "ticket.h"

#include <QUrlQuery>
#include <QNetworkReply>
#include <QStringList>

network::network(QObject *parent)
{
    this->setParent(parent);
    qId=1;
    baseUrl.setUrl("https://helpdesk.uic.edu/");

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
void network::Fetch(QUrl url){
    qDebug()<<"network::Fetch("<<url.toString()<<");";
    conn->Clear();
    conn->Get(url);
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
        qDebug()<<"emit(LoggedIn())";
        //essentially makes this the last time this function gets hit
        disconnect(conn, SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessData(QByteArray)));
        connect(conn, SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessREST(QByteArray)));
        emit(LoggedIn());
    }

}
void network::ProcessREST(QByteArray data){
    qDebug()<< "network::ProcessREST()";// << data;
    doc = QString(data);
}

void network::Load(){
    qDebug()<<"network::Load()";
    disconnect(this,SLOT(ProcessREST(QByteArray)));
    connect(conn, SIGNAL(httpDone(QByteArray)), this, SLOT(FindQueues(QByteArray)));
    FindQueues(NULL);
}
void network::GetTickets(QString queueName){
   disconnect(conn,SIGNAL(httpDone(QByteArray)),this, SLOT(ProcessREST(QByteArray)));
   connect(conn, SIGNAL(httpDone(QByteArray)), this, SLOT(ProcessTickets(QByteArray)));
   QUrl query;
   query.setUrl(baseUrl.toString().append(QString("las/REST/1.0/search/ticket?query=Queue='").append(queueName).append("' AND (Status %3D 'new' OR Status %3D 'open' OR Status %3D 'stalled')")  ));
   Fetch(query);
}

void network::ProcessTickets(QByteArray data){
    qDebug()<<data;
    QString reply(data);

    //make sure we have data
    if(reply.size()>0){
        QStringList lines = reply.split(QRegExp("\n\|\r\n\|\r"),QString::SkipEmptyParts);
        if(lines.at(0).contains("200 Ok")){
            if(lines.at(1).contains("No matching results.")){
                emit(Done());
                disconnect(conn,SIGNAL(httpDone(QByteArray)),this,SLOT(ProcessTickets(QByteArray)));
                return;
            }

            QString str;
            int loc;
            for(int i=1; i<lines.size(); i++){
                str = lines.at(i);
                loc = str.indexOf(':');
                if( str.contains(':') ){
                    QStringList strLst;
                    strLst.append(str.left(loc));
                    strLst.append(str.mid(loc+1).trimmed());
                    tickets.append( strLst );
                }
            }
            emit(Done());
            disconnect(conn,SIGNAL(httpDone(QByteArray)),this,SLOT(ProcessTickets(QByteArray)));
        }else{
            qWarning() << "REST error:" << reply;
            emit(Error("Error Getting Tickets",conn->reply->errorString()));
            disconnect(conn,SIGNAL(httpDone(QByteArray)),this,SLOT(ProcessTickets(QByteArray)));
            return;
        }
    }
}

void network::FindQueues(QByteArray data){
    qDebug()<<"network::FindQueues(QByteArray data)";
    QString reply(data);

    //make sure we have data
    if(reply.size()>0){
        qDebug()<<"network::FindQueues() has data";
        QStringList lines = reply.split(QRegExp("\n\|\r\n\|\r"),QString::SkipEmptyParts);
        if(lines.at(0).contains("200 Ok")){
            qDebug() << "network::FindQueues()" << lines.at(0);
            if(lines.at(1).contains("does not exist")){
                emit(Done());
                disconnect(conn,SIGNAL(httpDone(QByteArray)),this,SLOT(FindQueues(QByteArray)));
                disconnect(conn,SIGNAL(httpDone(QByteArray)),this,SLOT(ProcessREST(QByteArray)));
                return;
            }

            QString str;
            int loc;
            for(int i=1; i<lines.size(); i++){
                str = lines.at(i);
                loc = str.indexOf(':');
                if( (str.left(loc)=="Name") && (str.mid(loc+1).trimmed().size()>0) ){
                    queues[QString::number(qId)] = str.mid(loc+1).trimmed();
                    break;
                }
            }

            //increment if we can see the queue or not.
            qId++;
        }else{
            qWarning() << "REST error:" << reply;
            emit(Error("Error Finding Queues",conn->reply->errorString()));
            disconnect(conn,SIGNAL(httpDone(QByteArray)),this,SLOT(FindQueues(QByteArray)));
            disconnect(conn,SIGNAL(httpDone(QByteArray)),this,SLOT(ProcessREST(QByteArray)));
            return;
        }
    }
    QUrl queue;
    queue.setUrl(baseUrl.toString().append(QString("las/REST/1.0/queue/").append(QString::number(qId))));
    Fetch(queue);
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
