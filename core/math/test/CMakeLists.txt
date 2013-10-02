project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	test-core-math
	MathematicalEquationTest.cpp
)

target_link_libraries(
	test-core-math
	siox-core-MathematicalEquation
)

add_test(core-math test-core-math)
