project ( Predictor )


add_library ( prediction SHARED
	)

target_link_libraries ( prediction
	-L${SHOGUN_LIBRARY_DIR}
	libshogun.so
	tree_serialization
	ml_core
	)

SET_TARGET_PROPERTIES(prediction  PROPERTIES LINKER_LANGUAGE CXX)

#install (FILES PredictorInterface.hpp DESTINATION ${PREDICTOR_INSTALL_INCLUDE_DIR})
#install (FILES NeuralNetwork.hpp DESTINATION ${PREDICTOR_INSTALL_INCLUDE_DIR})
#install (FILES CART.hpp DESTINATION ${PREDICTOR_INSTALL_INCLUDE_DIR})
install (TARGETS prediction LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

#add_subdirectory( test )
