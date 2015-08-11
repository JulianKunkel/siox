project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-knowledge-ReasonerStandardImplementation
	ReasonerStandardImplementation.cpp
	ReasonerStandardImplementationOptions.cpp
	ReasonerCommunication.cpp
	ReasoningDatatypesSerializable.hpp
)

target_link_libraries(
	siox-knowledge-ReasonerStandardImplementation
	siox-core-AutoConfigurator
	siox-core-comm
	siox-monitoring-ThreadedStatisticsCollector
)

siox_run_binary_serializer( ReasonerCommunicationMessage.hpp ReasoningDatatypesSerializable.hpp )


siox_run_serializer(ReasonerStandardImplementationOptions.hpp ReasonerStandardImplementationOptions.cpp)

#add_subdirectory(test)

install(TARGETS siox-knowledge-ReasonerStandardImplementation LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
