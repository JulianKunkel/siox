project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-util
	autoLoadModules.cpp
)

target_link_libraries(
	siox-util
)


