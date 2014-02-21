project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${LIKWID_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsPlugin-likwid
	likwid-plugin.cpp
	ProviderLikwidOptions.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-likwid 
	siox-monitoring-statisticsPluginGeneric
	${LIKWID_LIBRARIES}
)

add_subdirectory(test)

siox_run_serializer(ProviderLikwidOptions.hpp ProviderLikwidOptions.cpp)


install(TARGETS siox-monitoring-statisticsPlugin-likwid LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
