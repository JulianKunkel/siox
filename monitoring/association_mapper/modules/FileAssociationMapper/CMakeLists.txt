project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-FileAssociationMapper
	FileAssociationMapper.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-FileAssociationMapper
	${Boost_LIBRARIES}
)

siox_run_serializer(FileAssociationMapperOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-FileAssociationMapper LIBRARY DESTINATION lib)
