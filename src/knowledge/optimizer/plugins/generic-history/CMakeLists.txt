project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)


add_library(
	siox-knowledge-optimizer-generic-history
	GenericHistory.cpp
	GenericHistoryOptions.cpp
)

target_link_libraries(
	siox-knowledge-optimizer-generic-history
	siox-core-AutoConfigurator
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(GenericHistoryOptions.hpp GenericHistoryOptions.cpp)

# add_subdirectory(test)

install(TARGETS siox-knowledge-optimizer-generic-history LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
