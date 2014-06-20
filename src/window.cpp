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

Window::Window(){
    QString helpdesk("https://helpdesk.uic.edu/las/");

    //setting up the main window

    //centralWidget = new QTextBrowser;
    //centralWidget->setPlainText(tr("Central widget"));

    BorderLayout *layout = new BorderLayout;

    //modifying how the table of settings looks
    queuesToWatch = new QListWidget();
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

    layout->addWidget(queuesToWatch, BorderLayout::Center);
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

    connection->Begin();
}
void Window::slotShowError(QString err,QString details){
    QMessageBox eBox(this);
    eBox.setWindowTitle("Error!");
    eBox.setText(err);
    eBox.setDetailedText(details);
    eBox.exec();
}
void Window::slotUpdate(){
    //refresh ui elements
    if(!isQueuesEnabledSetup){
        SetupQueueSettings();
    }
}

void Window::GetCredentials(){
    qDebug()<<"Window::GetCredentials()";
    loginDialog->exec();
}

void Window::slotAcceptUserLogin(QString &user, QString &pass){
    connection->SetCredentials(user, pass);
}

void Window::LoginComplete(){
    qDebug()<<"Window::LoginComplete";
    connection->Load();
}

void Window::SetupQueueSettings(){
    QList<QString> queueIds = connection->queues.keys();
    foreach(QString str, queueIds){
        //queueCheckBoxes->append(new QCheckBox(str,queueCheckBoxes));
        QListWidgetItem* item = new QListWidgetItem(connection->queues.value(str), queuesToWatch);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        queuesToWatch->addItem(item);
    }
    isQueuesEnabledSetup = true;
}

QLabel *Window::createLabel(const QString &text){
    QLabel *label = new QLabel(text);
    label->setFrameStyle(QFrame::Box | QFrame::Raised);
    return label;
}
