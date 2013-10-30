project(SIOX)

set(SIOX_INCLUDE_DIRS ${SIOX_INCLUDE_DIRS} ${SIOX_SOURCES}/monitoring)

add_subdirectory(activity_builder)
add_subdirectory(activity_multiplexer)
add_subdirectory(association_mapper)
add_subdirectory(low-level-c)
add_subdirectory(ontology)
add_subdirectory(statistics)
add_subdirectory(system_information)
add_subdirectory(transaction_system)
