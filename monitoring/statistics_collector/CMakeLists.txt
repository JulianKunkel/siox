cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
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

