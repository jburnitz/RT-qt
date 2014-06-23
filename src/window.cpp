#include "borderlayout.h"
#include "window.h"
#include "network.h"
#include "logindialog.h"

#include <QTextBrowser>
#include <QMessageBox>
#include <QLabel>
#include <QtNetwork>
#include <QListWidget>
#include <QCheckBox>
#include <QTreeWidget>

Window::Window(){
    QString helpdesk("https://helpdesk.uic.edu/las/");

    //setting up the main window

    //centralWidget = new QTextBrowser;
    //centralWidget->setPlainText(tr("Central widget"));

    BorderLayout *layout = new BorderLayout;

    //modifying how the table of settings looks
    queuesToWatch = new QListWidget();
    tree = new QTreeWidget(this);
    tree->setColumnCount(2);
    QList<QString> headers;
    headers.append("Ticket Number");
    headers.append("Subject");
    tree->setHeaderLabels(QStringList(headers));
    /*
    queuesToWatch->setHorizontalHeaderLabels(QStringList()<<"Queues"<<"Enabled");
    queuesToWatch->setShowGrid(false);
    queuesToWatch->verticalHeader()->setVisible(false);
    queuesToWatch->setWindowFlags(Qt::FramelessWindowHint);
    queuesToWatch->setAttribute(Qt::WA_TranslucentBackground);
    queuesToWatch->setStyleSheet("background:transparent");
    */

    isQueuesEnabledSetup = false;

    //queueCheckBoxes = new QList<QCheckBox*>();
    loginDialog = new LoginDialog( this );
    connection = new network(this);

    //header = createLabel("Ticket Number # ");
    status = createLabel(helpdesk);

    layout->addWidget(tree, BorderLayout::Center);
    //layout->addWidget(header, BorderLayout::North);
    layout->addWidget(status, BorderLayout::South);

    setLayout(layout);
    setWindowTitle(tr("RT"));

    QString user1("jburni2");
    loginDialog->setUsername( user1 );
    //loginDialog->setUsernamesList();

    connect(loginDialog,SIGNAL(acceptLogin(QString&,QString&,int&)), this, SLOT(slotAcceptUserLogin(QString&,QString&)));
    connect(connection, SIGNAL(CredentialsRequested()), this, SLOT(GetCredentials()));
    connect(connection, SIGNAL(LoggedIn()), this, SLOT(LoginComplete()));
    connect(connection, SIGNAL(Error(QString,QString)), this, SLOT(slotShowError(QString,QString)));
    connect(connection, SIGNAL(Done()), this, SLOT(slotUpdate()));
    connect(tree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(QueryItem(QTreeWidgetItem*,int)));

    connection->Begin();
}
void Window::slotShowError(QString err,QString details){
    QMessageBox eBox(this);
    eBox.setWindowTitle("Error!");
    eBox.setText(err);
    eBox.setDetailedText(details);
    status->setText("An Error has occurred");
    eBox.exec();
}
void Window::slotUpdate(){
    //refresh ui elements
    if(!isQueuesEnabledSetup){
        SetupQueueSettings();
    }
    status->setText("Ready.");
}

void Window::GetCredentials(){
    qDebug()<<"Window::GetCredentials()";
    loginDialog->exec();
}

void Window::slotAcceptUserLogin(QString &user, QString &pass){
    connection->SetCredentials(user, pass);
    status->setText("Logging in...");
}

void Window::LoginComplete(){
    qDebug()<<"Window::LoginComplete";
    status->setText("Refreshing Queues");
    connection->Load();
}

void Window::SetupQueueSettings(){
    QList<QString> queueIds = connection->queues.keys();
    QList<QTreeWidgetItem*> items;

    foreach(QString str, queueIds){
        items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(connection->queues.value(str))));
    }
    //items.back()->addChild(new QTreeWidgetItem((QTreeWidget*)0, QStringList("test")));
    tree->insertTopLevelItems(0, items);

    isQueuesEnabledSetup = true;
}
void Window::QueryItem(QTreeWidgetItem *item, int column){
    qDebug()<< "Window::QueryItem(QTreeWidgetItem *item, int column)";
    connection->tickets.clear();
    tempItem = item;
    connect(connection, SIGNAL(Done()), this, SLOT(AddChildren()));

    if(item->text(0).toInt()){
        qDebug()<<"Ticket Number:"<<item->text(0);
        status->setText(QString("Fetching ticket #").append(item->text(0)));
    }
    else{
        qDebug()<<"Queue:"<<item->text(0);
        status->setText(QString("Fetching tickets for ").append(item->text(0)));
    }

    connection->GetTickets(item->text(0));
}
void Window::AddChildren(){
    disconnect(connection, SIGNAL(Done()), this, SLOT(AddChildren()));
    //I dont want duplicates, I can do this smarter probably too...
    qDeleteAll(tempItem->takeChildren());
    foreach(QStringList strList, connection->tickets){
        tempItem->addChild(new QTreeWidgetItem(strList));
    }
    if(!tempItem->isExpanded()){
        tempItem->setExpanded(true);
    }
    status->setText("Ready.");
}

QLabel *Window::createLabel(const QString &text){
    QLabel *label = new QLabel(text);
    label->setFrameStyle(QFrame::Box | QFrame::Raised);
    return label;
}
