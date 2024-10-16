project(RinexTests LANGUAGES CXX)

add_executable(RinexTest
    main.cpp
)

target_link_libraries(RinexTest PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql RinexReader)
