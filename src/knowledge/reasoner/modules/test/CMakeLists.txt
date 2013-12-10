project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	knowledge-reasoner-modules-reasoner
	reasoner-test.cpp
)

target_link_libraries(
	knowledge-reasoner-modules-reasoner
	siox-core-ModuleLoader 
	siox-knowledge-ReasonerStandardImplementation
	siox-core-comm-gio
)

add_test(knowledge-reasoner-modules-reasoner knowledge-reasoner-modules-reasoner)