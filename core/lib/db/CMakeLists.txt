project(TestDB LANGUAGES CXX)

add_executable(TestDB
    facadedb.h
    facadedb.cpp
    TestDB.cpp

    scripts/rinex_sample.sql)


target_link_libraries(TestDB PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql RinexReader)
