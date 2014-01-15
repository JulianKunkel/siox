project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-TopologyAssociationMapper
	TopologyAssociationMapper.cpp
	TopologyAssociationMapperOptions.cpp
)

target_link_libraries(
	siox-monitoring-TopologyAssociationMapper
	${Boost_LIBRARIES}
)

siox_run_serializer(TopologyAssociationMapperOptions.hpp TopologyAssociationMapperOptions.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-TopologyAssociationMapper LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
