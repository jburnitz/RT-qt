QT += network widgets xml

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

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml
