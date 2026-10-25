find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(itch_tests
    smoke_test.cpp)

target_link_libraries(itch_tests
    PRIVATE
        itch::itch
        itch::warnings
        GTest::gtest_main)

# Registers each TEST() as its own ctest entry by querying the built binary.
gtest_discover_tests(itch_tests)
