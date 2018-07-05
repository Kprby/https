TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    https.cpp \
    ssl_client.cpp \
    ssl_client_buf.cpp \
    ssl_client_stream.cpp

LIBS+=-L. -lcrypto
LIBS+=-L. -lssl

HEADERS += \
    https.h \
    ssl_client.h \
    ssl_client_buf.h \
    ssl_client_stream.h
