project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsPlugin-DirectMSR
	DirectMSR-plugin.cpp
	ProviderDirectMSROptions.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-DirectMSR 
	siox-monitoring-statisticsPluginGeneric
)

add_subdirectory(test)

siox_run_serializer(ProviderDirectMSROptions.hpp ProviderDirectMSROptions.cpp)


install(TARGETS siox-monitoring-statisticsPlugin-DirectMSR LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
