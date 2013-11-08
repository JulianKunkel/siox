project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

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

add_subdirectory(plugins)
#add_subdirectory(test)

install(TARGETS siox-monitoring-ActivityMultiplexerAsync LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
