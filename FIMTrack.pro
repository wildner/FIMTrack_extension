#-------------------------------------------------
#
# Project created by QtCreator 2013-02-13T15:27:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = FIMTrack
TEMPLATE = app

include(Algorithm/algorithm.pri)
include(Data/data.pri)
include(Control/control.pri)
include(Utility/utility.pri)
include(Configuration/configuration.pri)
include(GUI/GUI.pri)
include(Main/main.pri)
include(Calculation/calculation.pri)

CONFIG  += app_bundle
CONFIG  += release

CONFIG(debug, debug|release) {
    DESTDIR     = build/debug/bin
    OBJECTS_DIR = build/debug
    MOC_DIR     = build/debug
    RCC_DIR     = build/debug
    UI_DIR      = build/debug
} else {
    DESTDIR     = build/release/bin
    OBJECTS_DIR = build/release
    MOC_DIR     = build/release
    RCC_DIR     = build/release
    UI_DIR      = build/release
}


macx {

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11

    QMAKE_CXXFLAGS += -stdlib=libc++ -Wall

    INCLUDEPATH += /usr/local/include

    LIBS += -L/usr/local/lib \
            -lopencv_core \
            -lopencv_highgui \
            -lopencv_imgproc

    QMAKE_CXXFLAGS_WARN_ON = -Wno-unused-variable -Wno-reorder
}

unix {

    QMAKE_CXXFLAGS += -std=c++11 -Wall -pedantic -Wno-unknown-pragmas

    INCLUDEPATH += /usr/include

    LIBS += -lopencv_core \
            -lopencv_highgui \
            -lopencv_imgproc

    QMAKE_CXXFLAGS_WARN_ON = -Wno-unused-variable -Wno-reorder
}

win32{

    INCLUDEPATH += C:\\OpenCV\\2.4.11\\build\\include

    QMAKE_LFLAGS += /INCREMENTAL:NO

    CONFIG(debug,debug|release){
        LIBS += C:\\OpenCV\\2.4.11\build\\x86\\vc10\\lib\\opencv_core2411d.lib
        LIBS += C:\\OpenCV\\2.4.11\build\\x86\\vc10\\lib\\opencv_highgui2411d.lib
        LIBS += C:\\OpenCV\\2.4.11\build\\x86\\vc10\\lib\\opencv_imgproc2411d.lib

    }

    CONFIG(release,debug|release){
        LIBS += C:\\OpenCV\\2.4.11\\build\\x86\\vc10\\lib\\opencv_core2411.lib
        LIBS += C:\\OpenCV\\2.4.11\\build\\x86\\vc10\\lib\\opencv_highgui2411.lib
        LIBS += C:\\OpenCV\\2.4.11\\build\\x86\\vc10\\lib\\opencv_imgproc2411.lib
    }
}
