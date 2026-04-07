QT -= gui
QT += network sql

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/server_controller.cpp \
    src/server_model.cpp \
    src/database.cpp

HEADERS += \
    src/server_controller.h \
    src/server_model.h \
    src/database.h

# Это важно для классов с Q_OBJECT
QT += core

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
