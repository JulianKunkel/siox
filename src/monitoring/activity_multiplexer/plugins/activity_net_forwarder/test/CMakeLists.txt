project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	monitoring_activity_multiplexer_plugins_activity_net_forwarder_test
	monitoring_activity_multiplexer_plugins_activity_net_forwarder_test.cpp
)

target_link_libraries(
	monitoring_activity_multiplexer_plugins_activity_net_forwarder_test
	siox-core-ModuleLoader
	siox-core-comm-gio
	siox-monitoring-activityPlugin-ActivityNetworkForwarder
)

add_test(monitoring_activity_multiplexer_plugins_activity_net_forwarder_test monitoring_activity_multiplexer_plugins_activity_net_forwarder_test)



