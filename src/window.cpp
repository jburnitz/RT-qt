#include "borderlayout.h"
#include "window.h"
#include "network.h"

#include <QTextBrowser>
#include <QLabel>
#include <QtNetwork>

Window::Window(){
    QString helpdesk("https://helpdesk.uic.edu/las/");

    centralWidget = new QTextBrowser;
    centralWidget->setPlainText(tr("Central widget"));

    BorderLayout *layout = new BorderLayout;
    layout->addWidget(centralWidget, BorderLayout::Center);
    header = createLabel("Ticket Number # ");
    layout->addWidget(header, BorderLayout::North);
   // layout->addWidget(createLabel("East 1"), BorderLayout::East);

    status = createLabel(helpdesk);
    layout->addWidget( status, BorderLayout::South);

    setLayout(layout);
    setWindowTitle(tr("RT"));

    QUrl url( helpdesk );

    connection = new network(this);
    connection->SetUrl(url);
    connection->Get();
}

void Window::ProcessData(QNetworkReply *reply){
    qDebug() << "Window::ProcessData";
    qDebug() << reply->url().host();
}
QLabel *Window::createLabel(const QString &text){
    QLabel *label = new QLabel(text);
    label->setFrameStyle(QFrame::Box | QFrame::Raised);
    return label;
}
