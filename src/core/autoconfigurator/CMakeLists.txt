project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-core-AutoConfigurator
	AutoConfigurator.cpp
   ModuleOptionsXML.hpp
)

target_link_libraries(
	siox-core-AutoConfigurator
	siox-core-ModuleLoader
   siox-core-ModuleLoader
	siox-core-ComponentRegistrar
   siox-core-ContainerSerializer	
)

siox_run_serializer(ModuleOptions.hpp ModuleOptionsXML.hpp)


add_subdirectory(ConfigurationProviderPlugins)

install(TARGETS siox-core-AutoConfigurator LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
