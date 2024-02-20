#-------------------------------------------------
#
# Project created by QtCreator 2021-04-01T22:51:36
#
#-------------------------------------------------
include (C:\Qt_projects\Ati_accounting\xslx\src\xlsx\qtxlsx.pri)

QT       += core gui sql axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#win32:RC_ICONS += file.ico

TARGET = ATI_accounting
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        entry.cpp \
    prodinfo.cpp \
        product.cpp \
    order.cpp \
    doc.cpp \
    human.cpp \
    organisation.cpp \
    additem.cpp \
    scanviewer.cpp \
    persaccount.cpp \
    additembn.cpp \
    tablewidgetitem.cpp \
    depart.cpp \
    orderdetails.cpp

HEADERS += \
        mainwindow.h \
        entry.h \
    prodinfo.h \
        product.h \
    order.h \
    doc.h \
    human.h \
    organisation.h \
    additem.h \
    scanviewer.h \
    persaccount.h \
    additembn.h \
    tablewidgetitem.h \
    depart.h \
    orderdetails.h

FORMS += \
        mainwindow.ui \
        entry.ui \
    additem.ui \
    prodinfo.ui \
    scanviewer.ui \
    additembn.ui \
    orderdetails.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32: RC_ICONS = $$PWD/icons/ATS.ico
