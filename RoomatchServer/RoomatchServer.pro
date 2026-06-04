QT += widgets sql

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    databasemanager.cpp \
    databaseworker.cpp \
    loginwidget.cpp \
    main.cpp \
    serverwindow.cpp

HEADERS += \
    databasemanager.h \
    databaseworker.h \
    loginwidget.h \
    serverwindow.h

FORMS += \
    loginwidget.ui \
    serverwindow.ui

TRANSLATIONS += \
    RoomatchServer_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
