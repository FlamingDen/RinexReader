project(RinexTests_DB LANGUAGES CXX)

add_executable(RinexTests_DB
    TestDB.cpp
)


target_link_libraries(RinexTests_DB PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql RinexReader)
