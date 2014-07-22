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
	-L${Boost_LIBRARY_DIR}
    ${Boost_LIBRARIES}
)

add_subdirectory(test)

install(TARGETS siox-core-autoconfigurator-FileConfigurationProvider LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
