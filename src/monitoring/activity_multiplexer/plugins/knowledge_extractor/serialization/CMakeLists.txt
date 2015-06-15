project ( Predictor )

add_library ( tree_serialization SHARED
	BasicTypesSerialization.cpp
	SGVectorSerialization.cpp
	CARTreeNodeDataSerialization.cpp
	CDynamicObjectArraySerialization.cpp
	CTreeMachineNodeSerialization.cpp
	CSGObjectPtrSerialization.cpp
	TreeSerialization.cpp
	NodeProxy.cpp
	)

target_link_libraries ( tree_serialization
	-L${SHOGUN_LIBRARY_DIR}
	libshogun.so
  )

#install (FILES BasicTypesSerialization.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
#install (FILES SGVectorSerialization.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
#install (FILES CARTreeNodeDataSerialization.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
#install (FILES CSGObjectPtrSerialization.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
#install (FILES CTreeMachineNodeSerialization.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
#install (FILES CDynamicObjectArraySerialization.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
#install (FILES TreeSerialization.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
#install (FILES 	NodeProxy.hpp DESTINATION ${SERIAL_INSTALL_INCLUDE_DIR})
install (TARGETS tree_serialization LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

#add_subdirectory( test )
