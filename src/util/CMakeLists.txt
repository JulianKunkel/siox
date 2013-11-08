project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-util
	autoLoadModules.cpp
	time.c
)

target_link_libraries(
	siox-util
	siox-core-AutoConfigurator
)

install(TARGETS siox-util LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

add_subdirectory(JobProcessors)
