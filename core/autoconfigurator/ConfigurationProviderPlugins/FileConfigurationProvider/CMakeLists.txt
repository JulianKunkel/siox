project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-core-autoconfigurator-FileConfigurationProvider
	FileConfigurationProvider.cpp
)

target_link_libraries(
	siox-core-autoconfigurator-FileConfigurationProvider
	siox-core-AutoConfigurator 
	siox-core-ContainerSerializer
)

add_subdirectory(test)
