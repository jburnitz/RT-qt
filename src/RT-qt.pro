QT += network widgets core

HEADERS     = borderlayout.h \
              window.h \
    network.h \
    http.h \
    logindialog.h \
    ticket.h
SOURCES     = borderlayout.cpp \
              main.cpp \
              window.cpp \
    network.cpp \
    http.cpp \
    logindialog.cpp \
    ticket.cpp

INSTALLS += target

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml
