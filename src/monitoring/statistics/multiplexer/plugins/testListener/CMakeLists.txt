project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsMultiplexerPlugin-testListener
	TestListener.cpp
)

target_link_libraries(
	siox-monitoring-statisticsMultiplexerPlugin-testListener
	siox-core-AutoConfigurator
)

# install(TARGETS  LIBRARY DESTINATION lib)
