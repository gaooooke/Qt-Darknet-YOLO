TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    improcess.cpp

HEADERS += \
    improcess.h \
    darknet.h

#opencv
INCLUDEPATH += /home/gaokechen/Downloads/opencv-3.3.1/include \
               /home/gaokechen/Downloads/opencv-3.3.1/include/opencv \
               /home/gaokechen/Downloads/opencv-3.3.1/include/opencv2/
LIBS += -L /home/gaokechen/Downloads/opencv-3.3.1/build/lib/libopencv_*.so

LIBS +=/home/gaokechen/darknet/libdarknet.so \
/usr/local/cuda-10.1/targets/x86_64-linux/lib/libcudart.so \
/usr/local/cuda-10.1/targets/x86_64-linux/lib/libcudnn.so \
/usr/local/cuda-10.1/targets/x86_64-linux/lib/libcurand.so \
/usr/lib/x86_64-linux-gnu/libcublas.so
