project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	ReferenceCountingTest
	ReferenceCountingTest.cpp
)

target_link_libraries(
	ReferenceCountingTest
)

add_test(util_ReferenceCountingTest ReferenceCountingTest)

