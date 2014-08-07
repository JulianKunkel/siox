# Find libpqxx.
#
# You might want to define:
# GLIB_ROOT_DIR
#
# This module defines:
# GLIB_FOUND
# GLIB_INCLUDE_DIRS
# GLIB_LIBRARIES
# 
# GIO_FOUND
# GIO_INCLUDE_DIRS
# GIO_LIBRARIES
#
# GMODULE_FOUND
# GMODULE_INCLUDE_DIRS
# GMODULE_LIBRARIES

set(GLIB_INCLUDE_PATH_DESCRIPTION "top-level directory containing the glib include directories. E.g /usr/local/include/glib-2.")
set(GLIB_INCLUDE_DIR_MESSAGE "Set the GLIB_INCLUDE_DIR cmake cache entry to the ${GLIB_INCLUDE_PATH_DESCRIPTION}.")
set(GLIB_LIBRARY_PATH_DESCRIPTION "top-level directory containing the glib libraries.")
set(GLIB_LIBRARY_DIR_MESSAGE "Set the GLIB_LIBRARY_DIR cmake cache entry to the ${GLIB_LIBRARY_PATH_DESCRIPTION}.")

if (NOT DEFINED GLIB_ROOT_DIR)
	if (NOT DEFINED GLIB_LIBRARIES)
		find_package(PkgConfig)

		# try to use pkg-config to localize the libraries.
		pkg_check_modules (GLIB2   glib-2.0>=2.36 REQUIRED)
		pkg_check_modules (GLIB2IO   gio-2.0 REQUIRED)
		pkg_check_modules (GLIB2UNIX  gio-unix-2.0 REQUIRED)

		set(GLIB_LIBRARIES ${GLIB2_LIBRARIES} ${GLIB2IO_LIBRARIES} ${GLIB2UNIX_LIBRARIES} CACHE STRING "GLIB LIBRARIES" )
		set(GLIB_INCLUDE_DIRS ${GLIB2_INCLUDE_DIRS} ${GLIB2IO_INCLUDE_DIRS} ${GLIB2UNIX_INCLUDE_DIRS} CACHE STRING "GLIB INCLUDE_DIRS")
	endif()
else(NOT DEFINED GLIB_ROOT_DIR)

	find_path(GLIB_INCLUDE
		NAME
			glib-2.0 
		PATHS
			${GLIB_ROOT_DIR}/include/
		DOC
			"Directory for glib headers"
		NO_DEFAULT_PATH
	)

	find_path(GLIB_MODULE_DIR
		NAME
			gmodule.h 
		PATHS
			${GLIB_ROOT_DIR}/include/glib-2.0/
		DOC
			"Directory for gmodule headers"
		NO_DEFAULT_PATH
	)

	find_path(GLIB_CONFIG_DIR
		NAME
			glibconfig.h
		PATHS
			${GLIB_ROOT_DIR}/lib/glib-2.0/include/
		DOC
			"Directory with the glibconfig.h"
		NO_DEFAULT_PATH
	)

	find_path(GLIB_IO_DIR
		NAME
			gio	
		PATHS
			${GLIB_ROOT_DIR}/include/gio-unix-2.0
		DOC
			"Directory with the gio stuff.h"
		NO_DEFAULT_PATH
	)

	set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE} ${GLIB_CONFIG_DIR} ${GLIB_IO_DIR} ${GLIB_MODULE_DIR})

	find_library(GLIB_LIBRARIES 
		NAMES 
			glib-2.0
		PATHS
			${GLIB_ROOT_DIR}/lib/
		DOC
			"Directory for glib binary"
		NO_DEFAULT_PATH
	)

	find_library(GLIB_IO 
		NAMES 
			gio-2.0 
		PATHS
			${GLIB_ROOT_DIR}/lib/
		DOC
			"Directory for gio binary"
		NO_DEFAULT_PATH
	)

	find_library(GLIB_OBJECT 
		NAMES 
			gobject-2.0
		PATHS
			${GLIB_ROOT_DIR}/lib/
		DOC
			"Directory for gobject binary"
		NO_DEFAULT_PATH
	)

	find_library(GLIB_MODULE
		NAMES 
			gmodule-2.0
		PATHS
			${GLIB_ROOT_DIR}/lib/
		DOC
			"Directory for gmodule binary"
		NO_DEFAULT_PATH
	)

	set(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_IO} ${GLIB_OBJECT} ${GLIB_MODULE})

endif(NOT DEFINED GLIB_ROOT_DIR)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Glib DEFAULT_MSG GLIB_INCLUDE_DIRS GLIB_LIBRARIES)

mark_as_advanced(GLIB_INCLUDE_DIRS GLIB_LIBRARIES)

