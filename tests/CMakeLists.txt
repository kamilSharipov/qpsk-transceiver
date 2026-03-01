find_package(GTest REQUIRED)

add_executable(qpsk_tests test_block_code.cpp)
target_link_libraries(qpsk_tests GTest::gtest GTest::gtest_main)
target_include_directories(qpsk_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party
)

add_test(NAME qpsk_tests COMMAND qpsk_tests)
