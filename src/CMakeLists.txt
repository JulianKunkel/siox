project(SIOX)

add_subdirectory(core)
add_subdirectory(knowledge)
add_subdirectory(monitoring)
add_subdirectory(util)
add_subdirectory(tools)

install(DIRECTORY include/ DESTINATION include/)

