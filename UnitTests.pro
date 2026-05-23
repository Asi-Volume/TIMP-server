QT += testlib core network sql
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SERVER_SRC_DIR = $$PWD/src

INCLUDEPATH += $$SERVER_SRC_DIR


SOURCES += \
        tst_funcforserver_test.cpp \
    $$SERVER_SRC_DIR/server_model.cpp \
    $$SERVER_SRC_DIR/server_controller.cpp \
    $$SERVER_SRC_DIR/database.cpp \
    $$SERVER_SRC_DIR/mailer.cpp

HEADERS += \
    $$SERVER_SRC_DIR/server_model.h \
    $$SERVER_SRC_DIR/server_controller.h \
    $$SERVER_SRC_DIR/database.h \
    $$SERVER_SRC_DIR/mailer.h
