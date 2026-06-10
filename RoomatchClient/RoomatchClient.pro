QT += widgets \
    network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    loginwidget.cpp \
    main.cpp \
    clientwindow.cpp \
    networkmanager.cpp \
    networkworker.cpp \
    studentregisterwidget.cpp

HEADERS += \
    clientwindow.h \
    loginwidget.h \
    networkmanager.h \
    networkworker.h \
    studentregisterwidget.h

FORMS += \
    clientwindow.ui \
    loginwidget.ui \
    studentregisterwidget.ui

TRANSLATIONS += \
    RoomatchClient_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 链接 RoomatchCommon 静态库
# 1. 告诉编译器去哪里找公共库的头文件 (.h)
INCLUDEPATH += $$PWD/../RoomatchCommon
DEPENDPATH  += $$PWD/../RoomatchCommon

# 2. 告诉编译器去哪里找静态库文件，并将其链接进来
# -L 参数指定库文件所在的文件夹路径（对应 Common 里的 DESTDIR）
# -l 参数指定库的名称（注意：去掉前缀 "lib" 和后缀 ".a"/".lib"）
LIBS += -L$$PWD/../RoomatchCommon/lib -lRoomatchCommon