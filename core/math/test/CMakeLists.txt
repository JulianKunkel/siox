project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(test-math
	MathematicalEquationTest.cpp
)

target_link_libraries(
	test-math
	siox-core-MathematicalEquation
)

add_test(Mathematics test-math)
