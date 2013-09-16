project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-core-AutoConfigurator
	AutoConfigurator.cpp
)

target_link_libraries(
	siox-core-AutoConfigurator
	siox-core-ModuleLoader 
	siox-core-ContainerSerializer 
	siox-core-ComponentRegistrar
	${Boost_LIBRARIES}
)

add_subdirectory(ConfigurationProviderPlugins)
