cmake_minimum_required(VERSION 2.8)
project(xante)

# Options
option(ALTERNATIVE_DIALOG "Uses an alternative libdialog to build/link." OFF)
option(DEBUG "Enable/Disable debug library" ON)
option(SHARED "Enable/Disable the shared library version" ON)

include_directories(include)
include_directories("include/xante")
include_directories("src/widgets")
include_directories("src/gadgets")
include_directories("src/libdialog")

if(CMAKE_C_COMPILER_VERSION VERSION_GREATER 5)
    add_definitions(-fgnu89-inline)
endif()

if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else(DEBUG)
    set(CMAKE_BUILD_TYPE Release)
endif(DEBUG)

add_definitions("-Wall -Wextra -fPIC")
add_definitions("-DLIBXANTE_COMPILE -D_GNU_SOURCE")

if(ALTERNATIVE_DIALOG)
    add_definitions("-DALTERNATIVE_DIALOG")
    include_directories("/usr/local/include/dialog")
endif(ALTERNATIVE_DIALOG)

file(GLOB SOURCES "src/*.c")
file(GLOB LIBDIALOG_SOURCES "src/libdialog/*.c")
file(GLOB WIDGETS_SOURCES "src/widgets/*.c")
file(GLOB GADGETS_SOURCES "src/gadgets/*.c")

set(SOURCE
    ${SOURCES}
    ${LIBDIALOG_SOURCES}
    ${WIDGETS_SOURCES}
    ${GADGETS_SOURCES})

set(VERSION_SCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib${PROJECT_NAME}.sym)

set(LIBRARY_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lib${PROJECT_NAME}.h)

execute_process(COMMAND grep MAJOR_VERSION ${LIBRARY_HEADER}
    COMMAND awk "{print $4}"
    COMMAND tr "\n" " "
    COMMAND sed "s/ //"
    OUTPUT_VARIABLE MAJOR_VERSION)

execute_process(COMMAND grep MINOR_VERSION ${LIBRARY_HEADER}
    COMMAND awk "{print $4}"
    COMMAND tr "\n" " "
    COMMAND sed "s/ //"
    OUTPUT_VARIABLE MINOR_VERSION)

execute_process(COMMAND grep RELEASE ${LIBRARY_HEADER}
    COMMAND awk "{print $4}"
    COMMAND tr "\n" " "
    COMMAND sed "s/ //"
    OUTPUT_VARIABLE RELEASE)

link_directories("/usr/local/lib")

if(SHARED)
    add_library(${PROJECT_NAME} SHARED ${SOURCE})
    target_link_libraries(${PROJECT_NAME} collections sqlite3 crypto dialog ncursesw)
    set_target_properties(${PROJECT_NAME} PROPERTIES
                          LINK_FLAGS "-Wl,-soname,${PROJECT_NAME}.so,--version-script,${VERSION_SCRIPT}")

    set_target_properties(${PROJECT_NAME} PROPERTIES
                          SUFFIX .so.${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE})
else(SHARED)
    add_library(${PROJECT_NAME} STATIC ${SOURCE})
endif(SHARED)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/lib)

