project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${GIOUNIX2_INCLUDE_DIRS}
)

add_library(
	siox-core-comm-multiMessageService
	MultiMessageTypeService.cpp
)

target_link_libraries(
	siox-core-comm-multiMessageService
)


add_subdirectory(modules)
add_subdirectory(test)

install(TARGETS siox-core-comm-multiMessageService LIBRARY DESTINATION lib)
