# - Try to find Likwid
# Once done this will define
#
#  LIKWID_FOUND - system has Likwid
#  LIKWID_INCLUDE_DIRS - the Likwid include directory
#  LIKWID_LIBRARIES - Link these to use Likwid.
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


IF (LIKWID_LIBRARIES AND LIKWID_INCLUDE_DIRS )
	# in cache already
	SET(LIKWID_FOUND TRUE)
ELSE (LIKWID_LIBRARIES AND LIKWID_INCLUDE_DIRS )

	INCLUDE(FindPkgConfig)

	IF ( LIKWID_FIND_REQUIRED )
		SET( _pkgconfig_REQUIRED "REQUIRED" )
	ELSE ( LIKWID_FIND_REQUIRED )
		SET( _pkgconfig_REQUIRED "" )
	ENDIF ( LIKWID_FIND_REQUIRED )

	IF ( PKG_CONFIG_FOUND )
		IF ( LIKWID_FOUND )
			SET ( LIKWID_CORE_FOUND TRUE )
		ELSE ( LIKWID_FOUND )
			SET ( LIKWID_CORE_FOUND FALSE )
		ENDIF ( LIKWID_FOUND )
	ENDIF ( PKG_CONFIG_FOUND )

	# Look for LIKWID include dir and libraries
	IF ( NOT LIKWID_FOUND )
		FIND_PATH(
			LIKWID_include_DIR
			NAMES
			likwid.h
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
			likwid/include
		)

		FIND_LIBRARY(
			LIKWID_link_DIR
			NAMES
			likwid
			PATHS
			/opt/local/lib
			/sw/lib
			/usr/lib
			/usr/local/lib
		)

		IF ( LIKWID_include_DIR AND LIKWID_link_DIR )
			SET ( LIKWID_FOUND TRUE )
		ENDIF ( LIKWID_include_DIR AND LIKWID_link_DIR )


		IF ( LIKWID_FOUND )
			SET ( LIKWID_INCLUDE_DIRS ${LIKWID_include_DIR} ${LIKWID_include_DIR}/likwid )
			SET ( LIKWID_LIBRARIES -L${LIKWID_link_DIR} -llikwid)
			SET ( LIKWID_CORE_FOUND TRUE )
		ELSE ( LIKWID_FOUND )
			SET ( LIKWID_CORE_FOUND FALSE )
		ENDIF ( LIKWID_FOUND )

	ENDIF ( NOT LIKWID_FOUND  )

	IF (LIKWID_CORE_FOUND AND LIKWID_INCLUDE_DIRS AND LIKWID_LIBRARIES)
		SET (LIKWID_FOUND TRUE)
	ENDIF (LIKWID_CORE_FOUND AND LIKWID_INCLUDE_DIRS AND LIKWID_LIBRARIES)

	IF (LIKWID_FOUND)
		IF (NOT LIKWID_FIND_QUIETLY)
			MESSAGE (STATUS "Found LIKWID: ${LIKWID_LIBRARIES} ${LIKWID_INCLUDE_DIRS}")
		ENDIF (NOT LIKWID_FIND_QUIETLY)
	ELSE (LIKWID_FOUND)
		IF (LIKWID_FIND_REQUIRED)
			MESSAGE (SEND_ERROR "Could not find LIKWID")
		ENDIF (LIKWID_FIND_REQUIRED)
	ENDIF (LIKWID_FOUND)

	# show the LIKWID_INCLUDE_DIRS and LIKWID_LIBRARIES variables only in the advanced view
	MARK_AS_ADVANCED(LIKWID_INCLUDE_DIRS LIKWID_LIBRARIES)

ENDIF (LIKWID_LIBRARIES AND LIKWID_INCLUDE_DIRS)

