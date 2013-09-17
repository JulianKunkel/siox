project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-core-comm-inmemory
	BoostXMLSerializable.cpp	
	CommInMemory.cpp
)

target_link_libraries(
	siox-core-comm-inmemory
)

# add_subdirectory(test)
