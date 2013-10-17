project(SIOX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpq")

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PostgreSQL_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityPostgreSQLWriter
	ActivityPostgreSQLWriter.cpp
	PostgreSQLQuerier.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityPostgreSQLWriter
	siox-core-ContainerSerializer
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

siox_run_serializer(ActivityPostgreSQLWriterOptions.hpp BoostXMLSerializable.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityPostgreSQLWriter LIBRARY DESTINATION lib)
