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

set(LIKWID_INCLUDE_PATH_DESCRIPTION "top-level directory containing the LIKWID include directories.")
set(LIKWID_INCLUDE_DIR_MESSAGE "Set the LIKWID_INCLUDE_DIR cmake cache entry to the ${LIKWID_INCLUDE_PATH_DESCRIPTION}")
set(LIKWID_LIBRARY_PATH_DESCRIPTION "top-level directory containing the LIKWID libraries.")
set(LIKWID_LIBRARY_DIR_MESSAGE "Set the LIKWID_LIBRARY_DIR cmake cache entry to the ${LIKWID_LIBRARY_PATH_DESCRIPTION}")

find_path(LIKWID_INCLUDE_DIRS 
        NAME
                likwid
        PATHS
                ${LIKWID_INCLUDE_DIR}
        DOC
                "Directory for LIKWID headers"
)


find_library(LIKWID_LIBRARIES 
        NAMES 
                likwid
        PATHS
                ${LIKWID_LIBRARY_DIR}
        DOC
                "Directory for LIKWID binary"
)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIKWID DEFAULT_MSG LIKWID_INCLUDE_DIRS LIKWID_LIBRARIES)

mark_as_advanced(LIKWID_INCLUDE_DIRS LIKWID_LIBRARIES)


