project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-core-comm-inmemory
	BoostXMLSerializable.cpp	
	CommInMemory.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-core-comm-inmemory
	${Boost_LIBRARIES}
	siox-core-comm
)

siox_run_serializer(CommInMemoryOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(test)
install(TARGETS siox-core-comm-inmemory LIBRARY DESTINATION lib)
