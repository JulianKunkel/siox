project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}	
)

add_library(
	siox-core-AutoConfigurator
	AutoConfigurator.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-core-AutoConfigurator
	siox-core-ModuleLoader 
	siox-core-ContainerSerializer 
	siox-core-ComponentRegistrar	
	${Boost_LIBRARIES}
)

siox_run_serializer(ModuleOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(ConfigurationProviderPlugins)

install(TARGETS siox-core-AutoConfigurator LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
