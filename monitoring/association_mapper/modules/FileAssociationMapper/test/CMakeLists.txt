cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(GLIB2 REQUIRED)
find_package(Boost 1.53.0 COMPONENTS serialization REQUIRED)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${Boost_INCLUDE_DIR}"
	"${GLIB2_INCLUDE_DIRS}"

)

add_executable(
	file-associationmapper-test
	file-associationmapper-test.cpp
)

target_link_libraries(
	file-associationmapper-test
	FileAssociationMapper
	module-loader
	"${Boost_LIBRARIES}"
	"${GLIB2_LIBRARIES}"
)


