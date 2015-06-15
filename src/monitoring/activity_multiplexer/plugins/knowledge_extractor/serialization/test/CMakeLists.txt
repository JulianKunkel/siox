project ( Predictor )

add_executable ( serializationtest
	serializationtest.cpp 
	#../functions.cpp
	)
target_link_libraries ( serializationtest
		-L${Shogun_LIBDIR}
		libshogun.so
		tree_serialization
 )

