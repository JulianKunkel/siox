project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-TopologySystemInformation
	TopologySystemInformation.cpp
	TopologySystemInformationOptions.cpp
)

target_link_libraries(
	siox-monitoring-TopologySystemInformation
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(TopologySystemInformationOptions.hpp TopologySystemInformationOptions.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-TopologySystemInformation LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
