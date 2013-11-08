project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-knowledge-DereferencingFacade
	DereferencingFacadeStandardImpl.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-knowledge-DereferencingFacade
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator
)

siox_run_serializer(DereferencingFacadeOptions.hpp BoostXMLSerializable.cpp)

install(TARGETS siox-knowledge-DereferencingFacade LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
