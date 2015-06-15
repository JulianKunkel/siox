
project ( Predictor )

#include_directories ()
#link_directories ()

#add_library()

add_executable ( classifiertest
	classifiertest.cpp 
	#../functions.cpp
	)
target_link_libraries ( classifiertest
		-L${Shogun_LIBDIR}
		libshogun.so
		classifier
 )

