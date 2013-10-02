project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	test-module-loader
	main.cpp
)

target_link_libraries(
	test-module-loader
	siox-core-ModuleLoader
	test-module
)

add_library(test-module
	test-module.cpp
)

target_link_libraries(
	test-module
	siox-core-ModuleLoader
)

add_test(Module-Loader test-module-loader)
