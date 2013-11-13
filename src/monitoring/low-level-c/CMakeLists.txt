project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
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
	siox-util
	${Boost_LIBRARIES}
)

add_subdirectory(examples)
add_subdirectory(test)

install(TARGETS siox-monitoring-siox-ll LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

