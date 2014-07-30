# Find libpqxx.
#
# You might want to define:
# PQXX_INCLUDE_DIR
# PQXX_LIBRARY_DIR
#
# This module defines:
# PQXX_FOUND
# PQXX_INCLUDE_DIRS
# PQXX_LIBRARIES

set(PQXX_INCLUDE_PATH_DESCRIPTION "top-level directory containing the PQXX include directories. E.g /usr/local/include/libpqxx-2.6.8 or C:/libpqxx-2.6.8")
set(PQXX_INCLUDE_DIR_MESSAGE "Set the PQXX_INCLUDE_DIR cmake cache entry to the ${PQXX_INCLUDE_PATH_DESCRIPTION}")
set(PQXX_LIBRARY_PATH_DESCRIPTION "top-level directory containing the PQXX libraries.")
set(PQXX_LIBRARY_DIR_MESSAGE "Set the PQXX_LIBRARY_DIR cmake cache entry to the ${PQXX_LIBRARY_PATH_DESCRIPTION}")

if (NOT DEFINED PQXX_INCLUDE_DIR)
	if (NOT DEFINED PQXX_LDFLAGS)
		find_package(PkgConfig)

		# try to use pkg-config to localize the libraries.
		pkg_check_modules (PQXX  libpqxx)

		set(PQXX_LIBRARIES ${PQXX_LIBRARIES} CACHE STRING "GLIB LDFLAGS" )
		set(PQXX_INCLUDE_DIRS ${PQXX_INCLUDE_DIRS}  CACHE STRING "GLIB CFLAGS")
	endif()
else()

	find_path(PQXX_INCLUDE_DIRS 
		NAME
			pqxx
		PATHS
			${PQXX_INCLUDE_DIR}
		DOC
			"Directory for pqxx headers"
	)


	find_library(PQXX_LIBRARIES 
		NAMES 
			pqxx
		PATHS
			${PQXX_LIBRARY_DIR}
		DOC
			"Directory for pqxx binary"
	)

endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Pqxx DEFAULT_MSG PQXX_INCLUDE_DIRS PQXX_LIBRARIES)

mark_as_advanced(PQXX_INCLUDE_DIRS PQXX_LIBRARIES)

