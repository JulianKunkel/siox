project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)
add_library(
	siox-monitoring-activityPlugin-PrintPlugin
	Print.cpp
	PrintOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-PrintPlugin
	siox-core-ModuleLoader
)

siox_run_serializer(PrintOptions.hpp PrintOptions.cpp)

install(TARGETS siox-monitoring-activityPlugin-PrintPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

