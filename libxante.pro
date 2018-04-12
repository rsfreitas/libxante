
CURRENT_ROOT_PATH = $$_PRO_FILE_PWD_

TEMPLATE = lib
TARGET = xante
DEPENDPATH += $$CURRENT_ROOT_PATH/include $$CURRENT_ROOT_PATH/src
INCLUDEPATH += $$CURRENT_ROOT_PATH/include

QT += core
LIBS +=
CONFIG += staticlib debug
DEFINES += LIBXANTE_COMPILE DEBUG

# Input
HEADERS += $$CURRENT_ROOT_PATH/include/cpp/config.hpp     \
        $$CURRENT_ROOT_PATH/include/cpp/jtf.hpp           \
        $$CURRENT_ROOT_PATH/include/cpp/item.hpp          \
        $$CURRENT_ROOT_PATH/include/cpp/menu.hpp

SOURCES += $$CURRENT_ROOT_PATH/src/cpp/config.cpp           \
            $$CURRENT_ROOT_PATH/src/cpp/jtf.cpp             \
            $$CURRENT_ROOT_PATH/src/cpp/item.cpp            \
            $$CURRENT_ROOT_PATH/src/cpp/menu.cpp

lib_dev_header.path = /usr/local/include/xante
lib_dev_header.files = $$CURRENT_ROOT_PATH/include/libxante.h
dev_headers.path = /usr/local/include/xante/cpp
dev_headers.files = $$CURRENT_ROOT_PATH/include/cpp/*.hpp
target.path = /usr/local/lib

INSTALLS += target lib_dev_header dev_headers

