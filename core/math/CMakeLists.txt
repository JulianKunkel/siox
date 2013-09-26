project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-core-MathematicalEquation
	MathematicalEquation.cpp
)

add_subdirectory(test)

install(TARGETS siox-core-MathematicalEquation LIBRARY DESTINATION lib)
