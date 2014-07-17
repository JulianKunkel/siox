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

find_path(GLIB_INCLUDE_DIRS 
	NAME
		glib-2.0 
	PATHS
		${GLIB_ROOT_DIR}/include/
	DOC
		"Directory for glib headers"
)

find_path(GLIB_CONFIG_DIR
	NAME
		glibconfig.h
	PATHS
		${GLIB_ROOT_DIR}/lib/glib-2.0/include/
	DOC
		"Directory with the glibconfig.h"
)

find_path(GLIB_IO_DIR
	NAME
		gio-unix-2.0
	PATHS
		${GLIB_ROOT_DIR}/include/
	DOC
		"Directory with the gio stuff.h"
)

set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIRS} ${GLIB_CONFIG_DIR} ${GLIB_IO_DIR})

find_library(GLIB_LIBRARIES 
	NAMES 
		glib-2.0
	PATHS
		${GLIB_ROOT_DIR}/lib/
	DOC
		"Directory for glib binary"
)

find_library(GLIB_IO 
	NAMES 
		gio-2.0 
	PATHS
		${GLIB_ROOT_DIR}/lib/
	DOC
		"Directory for gio binary"
)

find_library(GLIB_OBJECT 
	NAMES 
		gobject-2.0
	PATHS
		${GLIB_ROOT_DIR}/lib/
	DOC
		"Directory for gobject binary"
)

find_library(GLIB_MODULE
	NAMES 
		gmodule-2.0
	PATHS
		${GLIB_ROOT_DIR}/lib/
	DOC
		"Directory for gmodule binary"
)

set(GLIB_LIBRARIES ${GLIB_LIBRARIES} ${GLIB_IO} ${GLIB_OBJECT} ${GLIB_MODULE})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Glib DEFAULT_MSG GLIB_INCLUDE_DIRS GLIB_LIBRARIES)

mark_as_advanced(GLIB_INCLUDE_DIRS GLIB_LIBRARIES)

