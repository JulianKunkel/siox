project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsPlugin-providerskel
	ProviderSkeleton.cpp
	ProviderSkeletonOptions.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-providerskel
	siox-monitoring-statisticsPluginGeneric
)

siox_run_serializer(ProviderSkeletonOptions.hpp ProviderSkeletonOptions.cpp)


add_subdirectory(test)

# install(TARGETS  LIBRARY DESTINATION lib)
