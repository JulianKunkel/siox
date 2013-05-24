set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-glib2/")

find_package(GLIB2 REQUIRED)

include_directories(
	"${CMAKE_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/include/core/module/"
	"${GLIB2_INCLUDE_DIRS}"
)

message("${CMAKE_SOURCE_DIR}/include/core/module")

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11")

add_library(sioxmodloader
	module-loader.cpp
)

target_link_libraries(sioxmodloader ${LINK_LIBRARIES} ${GLIB2_LIBRARIES})
