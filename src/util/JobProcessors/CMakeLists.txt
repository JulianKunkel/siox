project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-util-jobProcessors
	DefaultProcessorQueues.cpp
	SingleThreadedJobProcessor.cpp
)

target_link_libraries(
	siox-util-jobProcessors
)

install(TARGETS siox-util-jobProcessors LIBRARY DESTINATION lib)
