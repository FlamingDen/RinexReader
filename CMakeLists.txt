cmake_minimum_required(VERSION 3.14)

project(RinexReader LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 Qt5 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

add_subdirectory(test)
add_subdirectory(db)

################################################################################################################
#--------------------------------------------------------------------------------------------------------------#
add_library(RinexReader
    src/csvcreator.cpp
    src/fileio.cpp
    src/pch.cpp
    src/rinex3nav.cpp
    src/rinex3obs.cpp
    src/rinexreader.cpp
    src/stringutils.cpp
    src/timeutils.cpp
    src/enumtypes.cpp
    src/rinexnamegenerator.cpp
    src/ireading.cpp

    include/rinexnamegenerator.h
    include/enumtypes.h
    include/csvcreator.h
    include/fileio.h
    include/pch.h
    include/rinex3nav.h
    include/rinex3obs.h
    include/rinexreader.h
    include/stringutils.h
    include/timeutils.h
    include/ireading.h
)

target_include_directories(RinexReader INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(RinexReader PRIVATE
    include
    src
)

target_link_libraries(RinexReader PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(RinexReader PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

#--------------------------------------------------------------------------------------------------------------#
################################################################################################################


