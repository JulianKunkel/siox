project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
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
)

target_link_libraries(
	test-serialize-modules 
	siox-core-AutoConfigurator
)

add_test(File-Configuration test-fileconfiguration)
