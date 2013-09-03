project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	ProviderSkeletonTest
	ProviderSkeletonTest.cpp
)

target_link_libraries(
	ProviderSkeletonTest
	siox-monitoring-activityPlugin-AnomalySkeleton 
	siox-core-AutoConfigurator
)

add_test(Provider-Skeleton ProviderSkeletonTest)

