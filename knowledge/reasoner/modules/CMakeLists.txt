project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-knowledge-ReasonerStandardImplementation
	ReasonerStandardImplementation.cpp
)

target_link_libraries(
	siox-knowledge-ReasonerStandardImplementation
	siox-core-AutoConfigurator
)

add_subdirectory(test)
