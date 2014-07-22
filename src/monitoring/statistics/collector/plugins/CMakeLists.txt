project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsPluginGeneric
	StatisticsProviderGeneric.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPluginGeneric
	siox-core-AutoConfigurator
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(StatisticsProviderGeneric.hpp StatisticsProviderGeneric.cpp)

install(TARGETS siox-monitoring-statisticsPluginGeneric LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

add_subdirectory(OSMemUsage)
add_subdirectory(cpustats)
#add_subdirectory(DirectMSR)
add_subdirectory(iostats)
add_subdirectory(network)
add_subdirectory(provider-skeleton)
add_subdirectory(provider-skeleton-minimal)
add_subdirectory(qualitativeUtilization)


IF ( LIKWID_FOUND )
	# We need a patched likwid version to use the likwid plugin
	add_subdirectory(likwid)
ENDIF ( LIKWID_FOUND )

