#include <mpi.h>
#include "siox-datatypes-helper-mpi.h"

#include <sys/types.h>
#include <unistd.h>
#include <glib.h>


static inline unsigned translateMPIOpenFlagsToSIOX( unsigned flags )
{
        return  ( ( flags & MPI_MODE_RDONLY ) > 0 ? SIOX_MPI_MODE_RDONLY : 0 ) |  ( ( flags & MPI_MODE_RDWR ) > 0 ? SIOX_MPI_MODE_RDWR : 0 ) |  ( ( flags & MPI_MODE_WRONLY ) > 0 ? SIOX_MPI_MODE_WRONLY : 0 ) |  ( ( flags & MPI_MODE_CREATE ) > 0 ? SIOX_MPI_MODE_CREATE : 0 ) |  ( ( flags & MPI_MODE_EXCL ) > 0 ? SIOX_MPI_MODE_EXCL : 0 ) |  ( ( flags & MPI_MODE_DELETE_ON_CLOSE ) > 0 ? SIOX_MPI_MODE_DELETE_ON_CLOSE : 0 ) |  ( ( flags & MPI_MODE_UNIQUE_OPEN ) > 0 ? SIOX_MPI_MODE_UNIQUE_OPEN : 0 ) |  ( ( flags & MPI_MODE_SEQUENTIAL ) > 0 ? SIOX_MPI_MODE_SEQUENTIAL : 0 ) |  ( ( flags & MPI_MODE_APPEND ) > 0 ? SIOX_MPI_MODE_APPEND : 0 );
}

static inline int translateMPIThreadLevelToSIOX( int flags )
{
        return  ( ( flags & MPI_THREAD_SINGLE ) > 0 ? SIOX_MPI_THREAD_SINGLE : 0 ) |  ( ( flags & MPI_THREAD_FUNNELED ) > 0 ? SIOX_MPI_THREAD_FUNNELED : 0 ) |  ( ( flags & MPI_THREAD_SERIALIZED ) > 0 ? SIOX_MPI_THREAD_SERIALIZED : 0 ) |  ( ( flags & MPI_THREAD_MULTIPLE ) > 0 ? SIOX_MPI_THREAD_MULTIPLE : 0 );
}

/*
	Record the recognized default info key/value pairs.
	Additionally, record all hints in a big string. Note that this string may contain not recognized hints.
 */
static inline void recordDefaultInfo(siox_activity * sioxActivity, MPI_Info info){
	if (info == MPI_INFO_NULL){
		return;
	}
	int number_of_keys = 0;
	MPI_Info_get_nkeys(info, & number_of_keys);

	// iterate over all hints, determine their size.
	unsigned stringLength = 0;
	for(int i=0; i < number_of_keys; i++){
		char key[MPI_MAX_INFO_KEY];
		char value[MPI_MAX_INFO_VAL];
		int isDefined;
		int ret;

		MPI_Info_get_nthkey(info, i, key);
		ret = MPI_Info_get(info, key, MPI_MAX_INFO_VAL, value, & isDefined);

		if ( ! isDefined || ret != MPI_SUCCESS ){
			continue;
		}		
		stringLength += strlen(key) + strlen(value) + 2;

		printf("[MPI] Hint: %s, %s\n", key, value);
	}
}

/* generated using the shell script
	for I in   ; do echo -n " ( ( flags & $I ) > 0 ? SIOX_$I : 0 ) | " ; done ; echo
*/

