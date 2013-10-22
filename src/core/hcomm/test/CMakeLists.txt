project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-hcomm-mmts-in-memory
	test-mmts-interfaces.cpp
)

target_link_libraries(
	core-hcomm-mmts-in-memory
	siox-core-hcomm-multiMessageService 
	siox-core-comm-inmemory 
	siox-core-ModuleLoader
	siox-core-comm
)

add_test(core-comm-mmts-in-memory core-comm-mmts-in-memory)


# add_executable(
# 	core-comm-multi-endpoint-client-test
# 	multi-endpoint-client-test.cpp
# )

# target_link_libraries(
# 	core-comm-multi-endpoint-client-test
# 	siox-core-ModuleLoader
# 	siox-core-comm-gio
# )

# add_test(core-comm-multi-endpoint-client-test core-comm-multi-endpoint-client-test)
