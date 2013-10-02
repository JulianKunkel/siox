project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_subdirectory(gio)
add_subdirectory(in-memory)
# add_subdirectory(asio) => Needs some more time to adjust it.
