project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	reasoner-test
	reasoner-test.cpp
)

target_link_libraries(
	reasoner-test
	siox-core-ModuleLoader 
	siox-knowledge-ReasonerStandardImplementation
)

add_test(Reasoner reasoner-test)