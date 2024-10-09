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

add_executable(RinexReader
    csvcreator.cpp
    facadedb.cpp
    fileio.cpp
    main.cpp
    pch.cpp
    rinex3nav.cpp
    rinex3obs.cpp
    rinexreader.cpp
    stringutils.cpp
    timeutils.cpp
    enumtypes.cpp
    rinexnamegenerator.cpp
    rinexnamegenerator.h
    enumtypes.h
    csvcreator.h
    facadedb.h
    fileio.h
    pch.h
    rinex3nav.h
    rinex3obs.h
    rinexreader.h
    stringutils.h
    timeutils.h
    ireading.h ireading.cpp
)

target_link_libraries(RinexReader PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(RinexReader PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
