project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)


add_library(
	siox-monitoring-activityPlugin-fadviseReadAhead
	fadviseReadAhead.cpp
	fadviseReadAheadOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-fadviseReadAhead
	siox-core-AutoConfigurator
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(fadviseReadAheadOptions.hpp fadviseReadAheadOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-fadviseReadAhead LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
