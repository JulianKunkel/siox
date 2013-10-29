#project(SIOX)
#
#include_directories(
#	${SIOX_INCLUDE_DIRS}
#	${Boost_INCLUDE_DIR}
#)
#
#add_library(
#	siox-monitoring-StatisticsMultiplexerSync
#	StatisticsMultiplexerSync.cpp
#)
#
#target_link_libraries(
#	siox-monitoring-StatisticsMultiplexerSync
#	siox-core-ContainerSerializer 
#	siox-core-AutoConfigurator
#	${Boost_LIBRARIES}
#)
#
#add_subdirectory(test)
#
#install(TARGETS siox-monitoring-StatisticsMultiplexerSync LIBRARY DESTINATION lib)
