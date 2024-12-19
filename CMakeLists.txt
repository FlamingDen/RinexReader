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

add_subdirectory(core)


################################################################################################################
#--------------------------------------------------------------------------------------------------------------#
add_library(RinexReader
    core/lib/src/csvcreator.cpp
    core/lib/src/fileio.cpp
    core/lib/src/pch.cpp
    core/lib/src/rinex3nav.cpp
    core/lib/src/rinex3obs.cpp
    core/lib/src/rinexreader.cpp
    core/lib/src/stringutils.cpp
    core/lib/src/timeutils.cpp
    core/lib/src/enumtypes.cpp
    core/lib/src/rinexnamegenerator.cpp
    core/lib/src/ireading.cpp
    core/lib/include/rinexnamegenerator.h
    core/lib/include/enumtypes.h
    core/lib/include/csvcreator.h
    core/lib/include/fileio.h
    core/lib/include/pch.h
    core/lib/include/rinex3nav.h
    core/lib/include/rinex3obs.h
    core/lib/include/rinexreader.h
    core/lib/include/stringutils.h
    core/lib/include/timeutils.h
    core/lib/include/ireading.h

    core/lib/models/pdp.h
    core/lib/models/pdp.cpp
    core/lib/models/condb.h
    core/lib/models/condb.cpp
    

    core/lib/db/facadedb.h
    core/lib/db/facadedb.cpp
)

target_include_directories(RinexReader INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lib/src
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lib/include

    ${CMAKE_CURRENT_SOURCE_DIR}/core/lib/models

    ${CMAKE_CURRENT_SOURCE_DIR}/core/lib/db
)

target_include_directories(RinexReader PRIVATE
    core/lib/include
    core/lib/src
    core/lib/models
    core/lib/db
)

target_link_libraries(RinexReader PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(RinexReader PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

#--------------------------------------------------------------------------------------------------------------#
################################################################################################################


