project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	AnomalySkeletonTest
	AnomalySkeletonTest.cpp
)

target_link_libraries(
	AnomalySkeletonTest
	siox-monitoring-activityPlugin-AnomalySkeletonTest
)

add_test(Anomaly-Skeleton AnomalySkeletonTest)

