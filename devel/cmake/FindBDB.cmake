# - Try to find Berkeley DB (> 4.4 with transaction support)
# Once done this will define
#
#  BDB_FOUND - system has BDB
#  BDB_INCLUDE_DIRS - the BDB include directory
#  BDB_LIBRARIES - Link these to use BDB.
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (BDB_LIBRARIES AND BDB_INCLUDE_DIRS )
	# in cache already
	SET(BDB_FOUND TRUE)
ELSE (BDB_LIBRARIES AND BDB_INCLUDE_DIRS )

	INCLUDE(FindPkgConfig)

	IF ( BDB_FIND_REQUIRED )
		SET( _pkgconfig_REQUIRED "REQUIRED" )
	ELSE ( BDB_FIND_REQUIRED )
		SET( _pkgconfig_REQUIRED "" )
	ENDIF ( BDB_FIND_REQUIRED )

	IF ( PKG_CONFIG_FOUND )
		IF ( BDB_FOUND )
			SET ( BDB_CORE_FOUND TRUE )
		ELSE ( BDB_FOUND )
			SET ( BDB_CORE_FOUND FALSE )
		ENDIF ( BDB_FOUND )
	ENDIF ( PKG_CONFIG_FOUND )

	# Look for BDB include dir and libraries
	IF ( NOT BDB_FOUND )
		FIND_PATH(
			BDB_include_DIR
			NAMES
			db.h
			perfmon.h
			PATHS
			/opt/lib/
			/opt/local/lib
			/sw/lib/
			/usr/lib64
			/usr/lib
			/usr/local/include
			${CMAKE_LIBRARY_PATH}
			PATH_SUFFIXES
			BDB/include
		)

		FIND_LIBRARY(
			BDB_link_DIR
			NAMES
			db
			PATHS
			/opt/local/lib
			/sw/lib
			/usr/lib
			/usr/local/lib
		)

		IF ( BDB_include_DIR AND BDB_link_DIR )
			SET ( BDB_FOUND TRUE )
		ENDIF ( BDB_include_DIR AND BDB_link_DIR )


		IF ( BDB_FOUND )
			SET ( BDB_INCLUDE_DIRS ${BDB_include_DIR} )
			SET ( BDB_LIBRARIES -L${BDB_link_DIR} -ldb)
			SET ( BDB_CORE_FOUND TRUE )
		ELSE ( BDB_FOUND )
			SET ( BDB_CORE_FOUND FALSE )
		ENDIF ( BDB_FOUND )

	ENDIF ( NOT BDB_FOUND  )

	IF (BDB_CORE_FOUND AND BDB_INCLUDE_DIRS AND BDB_LIBRARIES)
		SET (BDB_FOUND TRUE)
	ENDIF (BDB_CORE_FOUND AND BDB_INCLUDE_DIRS AND BDB_LIBRARIES)

	IF (BDB_FOUND)
		IF (NOT BDB_FIND_QUIETLY)
			MESSAGE (STATUS "Found BDB: ${BDB_LIBRARIES} ${BDB_INCLUDE_DIRS}")
		ENDIF (NOT BDB_FIND_QUIETLY)
	ELSE (BDB_FOUND)
		IF (BDB_FIND_REQUIRED)
			MESSAGE (SEND_ERROR "Could not find BDB")
		ENDIF (BDB_FIND_REQUIRED)
	ENDIF (BDB_FOUND)

	# show the BDB_INCLUDE_DIRS and BDB_LIBRARIES variables only in the advanced view
	MARK_AS_ADVANCED(BDB_INCLUDE_DIRS BDB_LIBRARIES)

ENDIF (BDB_LIBRARIES AND BDB_INCLUDE_DIRS)

