HEADERS += Base.h \
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

RESOURCES = res.qrc

TEMPLATE = app

icons.files= ios/Icon.png \
ios/Icon-120.png \
ios/Icon-152.png \
ios/Default-120.png \
ios/Default-152.png \
ios/Icon-72.png \
ios/Icon@2x.png \
ios/Icon-72@2x.png \
ios/Default.png \
ios/Default-72.png \
ios/Default@2x.png \
ios/Default-568h@2x.png \
ios/Info_ios.plist \
ios/bitmarket.storyboard

QMAKE_BUNDLE_DATA += icons

QMAKE_INFO_PLIST = ios/Info_ios.plist

TARGET = trader

DEFINES+= IPHONE NO_ICONV

QT += quick qml multimedia widgets concurrent

INCLUDEPATH+=./include -I/usr/local/opt/openssl/include -I/usr/local/opt/json-c/include -I/usr/local/opt/curl/include

#CONFIG+= debug
#CONFIG-= release

LIBS+= -lssl -lcrypto -lcurl -ljson-c -L/usr/local/opt/openssl -L/usr/local/opt/curl -L/usr/local/opt/json-c

include(QZXing/QZXing.pri)
