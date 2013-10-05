project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-FileBasedSystemInformation
	FileBasedSystemInformation.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-FileBasedSystemInformation
	${Boost_LIBRARIES}
)

siox_run_serializer(FileBasedSystemInformationOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-FileBasedSystemInformation LIBRARY DESTINATION lib)
