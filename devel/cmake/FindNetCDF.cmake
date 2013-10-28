# Copyright 2008-2009 NVIDIA Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License

FIND_PATH(NetCDF_INCLUDE_DIR netcdf.h HINTS ${NetCDF_INCLUDE_PATH})
FIND_LIBRARY(NetCDF_LIBRARY NAMES netcdf)

IF (NetCDF_INCLUDE_DIR AND NetCDF_LIBRARY)
	SET(NetCDF_FOUND TRUE)
ENDIF (NetCDF_INCLUDE_DIR AND NetCDF_LIBRARY)

IF (NetCDF_FOUND)
	IF (NOT NetCDF_FIND_QUIETLY)
		MESSAGE(STATUS "Found NETCDF: ${NetCDF_LIBRARY}")
	ENDIF (NOT NetCDF_FIND_QUIETLY)
ELSE (NetCDF_FOUND)
	IF (NetCDF_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could NOT find NetCDF")
	ELSE (NetCDF_FIND_REQUIRED)
		MESSAGE(STATUS "Could NOT find NetCDF")
	ENDIF (NetCDF_FIND_REQUIRED)
ENDIF (NetCDF_FOUND)

