HEADERS+=Base.h \
    Bitmarket.h \
    Bitmaszyna.h \
    key.h \
    uint256.h \
    allocators.h \
    util.h \
    table.h \
    basedata.h \
    wykres.h

SOURCES += main.cpp \
    Bitmarket.cpp \
    Bitmaszyna.cpp \
    key.cpp \
    Base.cpp \
    table.cpp \
    basedata.cpp \
    wykres.cpp

TEMPLATE = app

DEFINES+= DESKTOP

QT += quick qml multimedia widgets concurrent

INCLUDES+=./include

CONFIG+= c++11
CONFIG+= debug
CONFIG-= release
CONFIG += object_parallel_to_source

LIBS+= -lssl -lcrypto -lcurl -ljson-c

include(QZXing/QZXing.pri)

RESOURCES += res.qrc

#sudo apt-get install qtdeclarative5-qtmultimedia-plugin
