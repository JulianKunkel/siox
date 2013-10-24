project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

# build the rpc client and server test-modules
siox_run_serializer(rpc-client-module-options.hpp rpc-client-module-options.cpp)
siox_run_serializer(rpc-server-module-options.hpp rpc-server-module-options.cpp)

siox_run_binary_serializer(rpc-sample-interface.hpp rpc-sample-interfaceBinarySerializable.hpp)


add_library(
	siox-core-comm-rpc-client
	rpc-client-module.cpp	
	rpc-client-module-options.cpp
	rpc-sample-interfaceBinarySerializable.hpp
)

target_link_libraries(
	siox-core-comm-rpc-client
	siox-core-AutoConfigurator
	siox-core-comm
	${Boost_LIBRARIES}
)

add_library(
	siox-core-comm-rpc-server
	rpc-server-module.cpp
	rpc-server-module-options.cpp
	rpc-sample-interfaceBinarySerializable.hpp
)

target_link_libraries(
	siox-core-comm-rpc-server
	siox-core-AutoConfigurator
	siox-core-comm
	${Boost_LIBRARIES}
)


add_executable(
	core-comm-rpc-modules-test
	rpc-modules-test.cpp
)

target_link_libraries(
	core-comm-rpc-modules-test
	siox-core-comm-rpc-server
	siox-core-comm-rpc-client
	siox-core-ModuleLoader
	# link any module here...	
	siox-core-comm-gio
	${Boost_LIBRARIES}
)

add_test(core-comm-rpc-modules-test core-comm-rpc-modules-test)

