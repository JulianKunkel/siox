
project ( Predictor )

add_library ( ml_core SHARED
	functions.cpp
	)

target_link_libraries ( ml_core
	-L${SHOGUN_LIBRARY_DIR}
	libshogun.so
  )

#install (FILES functions.hpp DESTINATION ${SIOX_INSTALL_INCLUDE_DIR})
install (TARGETS ml_core LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

#add_subdirectory( test )
