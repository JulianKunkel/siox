project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${GIOUNIX2_INCLUDE_DIRS}
)

add_library(
	siox-core-hcomm-multiMessageService
	MultiMessageTypeService.cpp
)

target_link_libraries(
	siox-core-hcomm-multiMessageService
)

add_library(
	siox-core-hcomm	
	HighLevelCommunicationModuleOptions.cpp
	HighLevelCommunicationModule.cpp
)

target_link_libraries(
	siox-core-hcomm
)

siox_run_serializer(HighLevelCommunicationModuleOptions.hpp HighLevelCommunicationModuleOptions.cpp)


add_subdirectory(test)

install(TARGETS siox-core-hcomm-multiMessageService LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
install(TARGETS siox-core-hcomm LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
