QT += testlib core network sql
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../src

SOURCES += tst_funcforserver_test.cpp 


SOURCES += \
    ../src/server_model.cpp \
    ../src/database.cpp \
    ../src/mailer.cpp

HEADERS += \
    ../src/server_model.h \
    ../src/database.h \
    ../src/mailer.h