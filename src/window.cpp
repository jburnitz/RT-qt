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

    status = createLabel(helpdesk);
    layout->addWidget( status, BorderLayout::South);

    setLayout(layout);
    setWindowTitle(tr("RT"));

    //QUrl url( helpdesk );
    connection = new network(this);
}

QLabel *Window::createLabel(const QString &text){
    QLabel *label = new QLabel(text);
    label->setFrameStyle(QFrame::Box | QFrame::Raised);
    return label;
}
