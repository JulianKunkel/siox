# - Try to find libpqxx
# Once done this will define
#
#  libpqxx_FOUND - system has GLib2
#  libpqxx_INCLUDE_DIRS - the GLib2 include directory
#  libpqxx_LIBRARIES - Link these to use GLib2
#

IF ( libpqxx_LIBRARIES AND libpqxx_INCLUDE_DIRS )
	# in cache already
	SET( libpqxx_FOUND TRUE )
ELSE ( libpqxx_LIBRARIES AND libpqxx_INCLUDE_DIRS )

	INCLUDE(FindPkgConfig)

	## libpqxx
	IF ( libpqxx_FIND_REQUIRED )
		SET( _pkgconfig_REQUIRED "REQUIRED" )
	ELSE ( libpqxx_FIND_REQUIRED )
		SET( _pkgconfig_REQUIRED "" )
	ENDIF ( libpqxx_FIND_REQUIRED )

	IF ( libpqxx_MIN_VERSION )
		PKG_SEARCH_MODULE( libpqxx ${_pkgconfig_REQUIRED} libpqxx>=${libpqxx_MIN_VERSION} )
	ELSE ( libpqxx_MIN_VERSION )
		PKG_SEARCH_MODULE( libpqxx ${_pkgconfig_REQUIRED} libpqxx )
	ENDIF ( libpqxx_MIN_VERSION )

	IF ( libpqxx_FOUND )
		IF ( NOT libpqxx_FIND_QUIETLY )
			MESSAGE (STATUS "Found libpqxx: ${libpqxx_LIBRARIES} ${libpqxx_INCLUDE_DIRS}")
		ENDIF ( NOT libpqxx_FIND_QUIETLY )
	ELSE ( libpqxx_FOUND )
		IF ( libpqxx_FIND_REQUIRED )
			MESSAGE (SEND_ERROR "Could not find libpqxx")
		ENDIF ( libpqxx_FIND_REQUIRED )
	ENDIF ( libpqxx_FOUND )

ENDIF ( libpqxx_LIBRARIES AND libpqxx_INCLUDE_DIRS )

