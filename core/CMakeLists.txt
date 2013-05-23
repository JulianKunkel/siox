cmake_minimum_required(VERSION 2.8)
project(SIOX)

add_subdirectory(comm)
add_subdirectory(logger)
add_subdirectory(config)
add_subdirectory(tests)

enable_testing()
add_test("Communication" tests/communication_tests)
add_test("Configuration" tests/configuration_tests "${CMAKE_CURRENT_BIN_DIR}/siox.conf")
