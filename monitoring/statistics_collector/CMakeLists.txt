project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

# add_library(
# 	ThreadedStatisticsCollector
# 	ThreadedStatisticsCollector.cpp
# )
# 
# target_link_libraries(
# 	ThreadedStatisticsCollector
# )

add_subdirectory(plugins)
add_subdirectory(test)

