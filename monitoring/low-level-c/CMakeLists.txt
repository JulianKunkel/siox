project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-siox-ll
	siox-ll.cpp
)

target_link_libraries(
	siox-monitoring-siox-ll
	siox-core-ModuleLoader 
	siox-core-AutoConfigurator 
	siox-monitoring-ActivityBuilder
)

#add_subdirectory(examples)
#add_subdirectory(test)

