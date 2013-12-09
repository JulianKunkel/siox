project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-knowledge-ReasonerStandardImplementation
	ReasonerStandardImplementation.cpp
	ReasonerStandardImplementationOptions.cpp
)

target_link_libraries(
	siox-knowledge-ReasonerStandardImplementation
	siox-core-AutoConfigurator
	siox-core-comm
)

siox_run_serializer(ReasonerStandardImplementationOptions.hpp ReasonerStandardImplementationOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-knowledge-ReasonerStandardImplementation LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
