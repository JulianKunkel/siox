project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-knowledge-OptimizerStandardImplementation
	OptimizerStandardImplementation.cpp
	OptimizerOptions.cpp
)

target_link_libraries(
	siox-knowledge-OptimizerStandardImplementation
	siox-core-AutoConfigurator
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

add_subdirectory(test)

siox_run_serializer(OptimizerOptions.hpp OptimizerOptions.cpp)


install(TARGETS siox-knowledge-OptimizerStandardImplementation LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
