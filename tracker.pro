TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/include/AL

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lalut -lopenal
LIBS += -L/home/kim/ffmpeg/lib -lavcodec -lavformat
INCLUDEPATH += /home/brioche/ffmpeg/include

SOURCES += main.cpp \
    audioMaster.cpp \
    sourceAL.cpp

HEADERS += \
    audioMaster.h \
    sourceAL.h
