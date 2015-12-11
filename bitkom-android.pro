QT += widgets multimedia quick qml androidextras concurrent

DEFINES+= NO_ICONV ANDROID

HEADERS += Base.h \
    Bitmarket.h \
    Bitmaszyna.h \
    key.h \
    uint256.h \
    allocators.h \
    util.h \
    table.h \
    wykres.h \
    basedata.h

SOURCES += main.cpp \
    Bitmarket.cpp \
    Bitmaszyna.cpp \
    key.cpp \
    Base.cpp \
    table.cpp \
    wykres.cpp \
    basedata.cpp

LIBS+= -Llibs -lcurl -ljson

INCLUDEPATH += include

ANDROID_EXTRA_LIBS += libs/libcurl.so libs/libjson.so

include(QZXing/QZXing.pri)

RESOURCES += res.qrc

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/src/com/bitkom/NotificationClient.java \
    android/res/drawable/icon.png

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
