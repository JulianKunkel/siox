project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	file-associationmapper-test
	file-associationmapper-test.cpp
)

target_link_libraries(
	file-associationmapper-test
	siox-core-ModuleLoader 
	siox-monitoring-FileAssociationMapper
)

add_test(File-Association-Mapper file-associationmapper-test)

