cmake_minimum_required(VERSION 2.8)
project(SIOX)

add_subdirectory(core)
#add_subdirectory(knowledge)
add_subdirectory(monitoring)
add_subdirectory(tests)

enable_testing()
add_test("Communication" tests/core/comm/communication_tests)
add_test("PostgreSQL"    tests/monitoring/transaction_system/postgresql_test)
