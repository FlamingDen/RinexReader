project(RinexTests_Just LANGUAGES CXX)

add_executable(RinexTests_Just
    everyday.cpp
)

target_link_libraries(RinexTests_Just PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql RinexReader)
