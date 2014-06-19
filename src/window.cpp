#include "borderlayout.h"
#include "window.h"
#include "network.h"
#include "logindialog.h"

#include <QTextBrowser>
#include <QLabel>
#include <QtNetwork>

Window::Window(){
    QString helpdesk("https://helpdesk.uic.edu/las/");

    //setting up the main window
    centralWidget = new QTextBrowser;
    centralWidget->setPlainText(tr("Central widget"));

    BorderLayout *layout = new BorderLayout;
    layout->addWidget(centralWidget, BorderLayout::Center);
    header = createLabel("Ticket Number # ");
    layout->addWidget(header, BorderLayout::North);

    status = createLabel(helpdesk);
    layout->addWidget( status, BorderLayout::South);

    setLayout(layout);
    setWindowTitle(tr("RT"));

    //set up login dialog
    loginDialog = new LoginDialog( this );
    QString user1("jburni2");
    loginDialog->setUsername( user1 );
    connect( loginDialog,SIGNAL(acceptLogin(QString&,QString&,int&)), this, SLOT(slotAcceptUserLogin(QString&,QString&)));

    connection = new network(this);
    connect(connection, SIGNAL(CredentialsRequested()), this, SLOT(GetCredentials()));
    connect(connection, SIGNAL(LoggedIn()), this, SLOT(LoginComplete()));

    connection->Begin();

    //connect( centralWidget, SIGNAL(anchorClicked(QUrl)) )
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
    //centralWidget->setHtml(connection->doc);
    //connection->Fetch("https://helpdesk.uic.edu/las/REST/1.0/search/ticket?query=Status%20%3D%20%27open%27%20OR%20Status%20=%20%27new%27");
}

QLabel *Window::createLabel(const QString &text){
    QLabel *label = new QLabel(text);
    label->setFrameStyle(QFrame::Box | QFrame::Raised);
    return label;
}
