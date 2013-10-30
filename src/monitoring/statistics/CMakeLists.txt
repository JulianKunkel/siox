project(SIOX)

set(SIOX_INCLUDE_DIRS ${SIOX_INCLUDE_DIRS} ${SIOX_SOURCES}/statistics)

add_subdirectory(collector)
add_subdirectory(multiplexer)
add_subdirectory(test)
