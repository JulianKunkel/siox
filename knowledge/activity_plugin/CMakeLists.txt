project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-knowledge-DereferencingFacade
	ActivityPluginDereferencingStandardImpl.cpp
)

target_link_libraries(
	siox-knowledge-DereferencingFacade
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator
)