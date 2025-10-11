QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    admini.cpp \
    adminiroom.cpp \
    cloudRoom.cpp \
    main.cpp \
    mainwindow.cpp \
    relics.cpp \
    relicsroom.cpp \
    tlogindialog.cpp

HEADERS += \
    admini.h \
    adminiroom.h \
    cloudRoom.h \
    mainwindow.h \
    relics.h \
    relicsroom.h \
    tlogindialog.h

FORMS += \
    mainwindow.ui \
    tlogindialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
