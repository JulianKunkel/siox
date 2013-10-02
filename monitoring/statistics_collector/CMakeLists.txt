project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

# add_library(
# 	ThreadedStatisticsCollector
# 	ThreadedStatisticsCollector.cpp
# )
# 
# target_link_libraries(
# 	ThreadedStatisticsCollector
# )

siox_run_serializer(ThreadedStatisticsOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(plugins)
add_subdirectory(test)

install(TARGETS  LIBRARY DESTINATION lib)
