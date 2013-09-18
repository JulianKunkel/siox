project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

#add_custom_command(
#	OUTPUT BoostXMLSerializable.cpp
#	COMMAND ${CMAKE_SOURCE_DIR}/tools/serialization-code-generator.py
#	ARGS -I ${CMAKE_SOURCE_DIR}/include -i ${CMAKE_CURRENT_SOURCE_DIR}/ActivityMultiplexerAsyncOptions.hpp -o BoostXMLSerializable.cpp
#)

siox_run_serializer(ActivityMultiplexerAsyncOptions.hpp BoostXMLSerializable.cpp)

add_library(
	siox-monitoring-ActivityMultiplexerAsync
	ActivityMultiplexerAsync.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-ActivityMultiplexerAsync
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator 
	${Boost_LIBRARIES}
)

#add_subdirectory(plugins)
#add_subdirectory(test)

