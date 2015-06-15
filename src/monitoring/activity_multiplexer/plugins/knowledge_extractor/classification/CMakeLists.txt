
project ( Predictor )

add_library ( classifier SHARED
	)

target_link_libraries ( classifier
	-L${SHOGUN_LIBRARY_DIR}
	libshogun.so
	tree_serialization
	ml_core
	)

SET_TARGET_PROPERTIES(classifier PROPERTIES LINKER_LANGUAGE CXX)

#install (FILES Classifier.hpp DESTINATION ${CLASSIFIER_INSTALL_INCLUDE_DIR})
install (TARGETS classifier LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

#add_subdirectory( test )
