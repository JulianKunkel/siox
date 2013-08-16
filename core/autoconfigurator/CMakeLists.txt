cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS serialization REQUIRED)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)

add_library(
	AutoConfigurator
	AutoConfigurator.cpp
)

target_link_libraries(
	AutoConfigurator
	core-component-registrar
	core-container-serializer
	${Boost_LIBRARIES}
)

add_subdirectory(ConfigurationProviderPlugins)
