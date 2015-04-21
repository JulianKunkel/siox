project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-core-AutoConfigurator
	AutoConfigurator.cpp
	ModuleOptionsXML.cpp
)

target_link_libraries(
	siox-core-AutoConfigurator
	siox-core-ModuleLoader 
	siox-core-ContainerSerializer 
	siox-core-ComponentRegistrar	
)

siox_run_serializer(ModuleOptions.hpp ModuleOptionsXML.cpp)


add_subdirectory(ConfigurationProviderPlugins)

install(TARGETS siox-core-AutoConfigurator LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
