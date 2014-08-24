project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)


add_library(
	siox-knowledge-optimizer-persistent-store
	PersistentStore.cpp
	PersistentStoreOptions.cpp
)

target_link_libraries(
	siox-knowledge-optimizer-persistent-store
	siox-core-AutoConfigurator
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(PersistentStoreOptions.hpp PersistentStoreOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-knowledge-optimizer-persistent-store LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
