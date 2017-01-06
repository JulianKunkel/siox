project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	#${Boost_INCLUDE_DIR}
)
add_library(
	siox-monitoring-activityPlugin-FileAccessInfoPlugin
	FileAccessInfo.cpp
	FileAccessInfoOptions.cpp
	#datatypes/Basics.cpp
	#datatypes/BaseDatapoint.cpp
	#datatypes/TSDBDatapoint.cpp
	#datatypes/ElasticDatapoint.cpp
	clients/Basics.cpp
	clients/BaseClient.cpp
	clients/TSDBClient.cpp
	clients/ElasticClient.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-FileAccessInfoPlugin
	siox-core-ModuleLoader
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(FileAccessInfoOptions.hpp FileAccessInfoOptions.cpp)

install(TARGETS siox-monitoring-activityPlugin-FileAccessInfoPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

