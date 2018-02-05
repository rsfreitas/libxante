
TEMPLATE = lib
TARGET = xante
DEPENDPATH += ../../include src
INCLUDEPATH += ../../include

QT += core
LIBS +=
CONFIG += staticlib debug
DEFINES += LIBXANTE_COMPILE

# Input
HEADERS += ../../include/cpp/config.hpp     \
        ../../include/cpp/jtf.hpp           \
        ../../include/cpp/item.hpp          \
        ../../include/cpp/menu.hpp

SOURCES += config.cpp           \
            jtf.cpp             \
            item.cpp            \
            menu.cpp
 
