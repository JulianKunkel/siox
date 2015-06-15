project ( Predictor )

#include_directories ()
#link_directories ()

#add_library()

add_executable ( predictortest
	predictortest.cpp 
	#../functions.cpp
	)
target_link_libraries (
		predictortest
		-L${Shogun_LIBDIR}
		libshogun.so
		prediction
 )

