project (SIOX)

include_directories (
	${SIOX_INCLUDE_DIR}
	#${Predictor_INCLUDE_DIR}
	${SHOGUN_INCLUDE_DIR}
	)

add_library ( siox-monitoring-activityPlugin-KnowledgeExtractor SHARED
	KnowledgeExtractor.cpp
	KnowledgeExtractorOptions.cpp
	)

target_link_libraries ( siox-monitoring-activityPlugin-KnowledgeExtractor
	siox-core-ModuleLoader
	siox-core-AutoConfigurator
	prediction
	${Boost_LIBRARIES}
	)

siox_run_serializer( KnowledgeExtractorOptions.hpp KnowledgeExtractorOptions.cpp )

install (TARGETS siox-monitoring-activityPlugin-KnowledgeExtractor LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

#add_subdirectory( test )
add_subdirectory(core)
add_subdirectory(predictor)
add_subdirectory(classification)
add_subdirectory(serialization)
