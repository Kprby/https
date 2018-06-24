TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    https.cpp \
    ssl_client.cpp

LIBS+=-L. -lcrypto
LIBS+=-L. -lssl

HEADERS += \
    https.h \
    ssl_client.h
