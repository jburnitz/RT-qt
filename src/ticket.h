#ifndef TICKET_H
#define TICKET_H

#include <QObject>
#include <QMap>

class ticket : public QObject
{
    Q_OBJECT
public:
    explicit ticket(QObject *parent = 0);
    ticket(QString &restData);
    void Fill(QString &restData);

signals:

public slots:

protected:
    QMap<QString, QString> map;

};

#endif // TICKET_H
