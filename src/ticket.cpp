#include "ticket.h"

#include <QMap>
#include <QStringList>
#include <QDebug>

ticket::ticket(QObject *parent) :
    QObject(parent)
{
}
ticket::ticket(QString &restData){
    qDebug() << "ticket::ticket(QString)";
    map.clear();
    Fill( restData );
}

void ticket::Fill(QString &restData){
    qDebug() << "ticket::Fill()";
    QStringList members = restData.split(QRegExp("\n\|\r\n\|\r"),QString::SkipEmptyParts);
    if(members.at(0).contains("200 Ok")){
        qDebug() << "ticket::Fill() Valid REST";
        QString str;
        int loc;
        for(int i=1; i<members.size(); i++){
            str = members.at(i);
            loc = str.indexOf(':');
            map[str.left(loc)] = str.mid(loc+1).trimmed();
        }
        //qDebug()<< map["Subject"];
    }
}
