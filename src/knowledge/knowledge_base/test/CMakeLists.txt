project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	systeminfo_test
	systeminfo_test.cpp
)

target_link_libraries(
	systeminfo_test
	siox-knowledge-SystemInfoPersistency
)

add_test(System-Info systeminfo_test)

