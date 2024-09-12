QT = core sql

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        csvcreator.cpp \
        facadedb.cpp \
        fileio.cpp \
        main.cpp \
        pch.cpp \
        rinex3nav.cpp \
        rinex3obs.cpp \
        rinexreader.cpp \
        stringutils.cpp \
        timeutils.cpp



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    csvcreator.h \
    facadedb.h \
    fileio.h \
    pch.h \
    rinex3nav.h \
    rinex3obs.h \
    rinexreader.h \
    stringutils.h \
    timeutils.h

RESOURCES +=
