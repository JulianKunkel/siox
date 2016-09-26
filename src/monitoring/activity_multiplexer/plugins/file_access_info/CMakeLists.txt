project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	#${Boost_INCLUDE_DIR}
)
add_library(
	siox-monitoring-activityPlugin-FileAccessInfoPlugin
	FileAccessInfo.cpp
	FileAccessInfoOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-FileAccessInfoPlugin
	siox-core-ModuleLoader
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(FileAccessInfoOptions.hpp FileAccessInfoOptions.cpp)

install(TARGETS siox-monitoring-activityPlugin-FileAccessInfoPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

