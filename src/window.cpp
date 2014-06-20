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
    tree->setHeaderLabel("Queues");
    tree->setColumnCount(1);
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

    header = createLabel("Ticket Number # ");
    status = createLabel(helpdesk);

    layout->addWidget(tree, BorderLayout::Center);
    layout->addWidget(header, BorderLayout::North);
    layout->addWidget(status, BorderLayout::South);

    setLayout(layout);
    setWindowTitle(tr("RT"));

    QString user1("jburni2");
    loginDialog->setUsername( user1 );

    connect(loginDialog,SIGNAL(acceptLogin(QString&,QString&,int&)), this, SLOT(slotAcceptUserLogin(QString&,QString&)));
    connect(connection, SIGNAL(CredentialsRequested()), this, SLOT(GetCredentials()));
    connect(connection, SIGNAL(LoggedIn()), this, SLOT(LoginComplete()));
    connect(connection, SIGNAL(Error(QString,QString)), this, SLOT(slotShowError(QString,QString)));
    connect(connection, SIGNAL(Done()), this, SLOT(slotUpdate()));
    //connect(queuesToWatch, SIGNAL())

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
        status->setText("Refreshing Queues");
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
    status->setText("Logged in!");
    connection->Load();
}

void Window::SetupQueueSettings(){
    QList<QString> queueIds = connection->queues.keys();
    QList<QTreeWidgetItem*> items;

    foreach(QString str, queueIds){
        items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(connection->queues.value(str))));
    }
    tree->insertTopLevelItems(0, items);

    isQueuesEnabledSetup = true;
}

QLabel *Window::createLabel(const QString &text){
    QLabel *label = new QLabel(text);
    label->setFrameStyle(QFrame::Box | QFrame::Raised);
    return label;
}
