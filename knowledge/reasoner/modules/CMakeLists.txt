project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-knowledge-ReasonerStandardImplementation
	ReasonerStandardImplementation.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-knowledge-ReasonerStandardImplementation
	siox-core-AutoConfigurator
)

siox_run_serializer(ReasonerStandardImplementationOptions.hpp BoostXMLSerializable.cpp)

add_subdirectory(test)

