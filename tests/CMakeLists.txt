find_package(GTest REQUIRED)

add_executable(qpsk_tests
    test_block_code.cpp
    test_qpsk.cpp
    test_channel.cpp
)

target_link_libraries(qpsk_tests
    GTest::gtest
    GTest::gtest_main
    qpsk_core
)

target_include_directories(qpsk_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party
)

add_test(NAME qpsk_tests COMMAND qpsk_tests)
