project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_executable(
	test-fileconfiguration
	fileconfiguration-test.cpp
)
target_link_libraries(
	test-fileconfiguration
	test-serialize-modules
	siox-core-AutoConfigurator
)

add_library(
	test-serialize-modules
	test-serialize-modules.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	test-serialize-modules 
	siox-core-autoconfigurator-FileConfigurationProvider
)

siox_run_serializer(test-serialize-modules.hpp BoostXMLSerializable.cpp)

symlink(test.config test.config)


add_test(File-Configuration test-fileconfiguration)
