cmake_minimum_required(VERSION 2.8)
project(SIOX)

add_subdirectory(core)
#add_subdirectory(knowledge)
add_subdirectory(monitoring)

# enable_testing()
# add_test("Communication" core/test/communication_tests)
# add_test("Communication" monitoring/transaction_system/test/postgresql_tests)
