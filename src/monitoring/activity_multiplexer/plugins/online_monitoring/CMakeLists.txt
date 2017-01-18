project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	#${Boost_INCLUDE_DIR}
)
add_library(
	siox-monitoring-activityPlugin-OnlineMonitoringPlugin
	OnlineMonitoring.cpp
	OnlineMonitoringOptions.cpp
	clients/Basics.cpp
	clients/BaseClient.cpp
	clients/TSDBClient.cpp
	clients/ElasticClient.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-OnlineMonitoringPlugin
	siox-core-ModuleLoader
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(OnlineMonitoringOptions.hpp OnlineMonitoringOptions.cpp)

install(TARGETS siox-monitoring-activityPlugin-OnlineMonitoringPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

