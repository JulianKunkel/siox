project(SIOX)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(core)
add_subdirectory(monitoring)

enable_testing()
add_test("Communication" core/test/communication_tests)
add_test("Configuration" core/test/configuration_tests "${CMAKE_SOURCE_DIR}/core/test/siox.conf")
