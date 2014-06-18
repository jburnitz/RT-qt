#ifndef TICKET_H
#define TICKET_H

#include <QObject>

class ticket : public QObject
{
    Q_OBJECT
public:
    explicit ticket(QObject *parent = 0);
    ticket(QString &restData);

signals:

public slots:

};

#endif // TICKET_H
