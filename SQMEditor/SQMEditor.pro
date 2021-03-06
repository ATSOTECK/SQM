QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AddRemoveText.cpp \
    CentralDocument.cpp \
    CentralWidget.cpp \
    CodeEditor.cpp \
    ConsoleDock.cpp \
    FSDock.cpp \
    HelpDialog.cpp \
    Highlighter.cpp \
    MiniMap.cpp \
    ResultsDock.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    AddRemoveText.h \
    CentralDocument.h \
    CentralWidget.h \
    CodeEditor.h \
    ConsoleDock.h \
    FSDock.h \
    HelpDialog.h \
    Highlighter.h \
    MainWindow.h \
    MiniMap.h \
    ResultsDock.h \
    common.h

FORMS += \
    HelpDialog.ui \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    style.css \
    wordlist.txt
