QT += network \
     widgets \
    xml

HEADERS     = borderlayout.h \
              window.h \
    network.h \
    http.h \
    logindialog.h
SOURCES     = borderlayout.cpp \
              main.cpp \
              window.cpp \
    network.cpp \
    http.cpp \
    logindialog.cpp

INSTALLS += target
