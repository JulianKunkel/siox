#include <regex>
#include <unordered_map>
#include <iostream>
#include <string>

#include <util/time.h>
#include <monitoring/ontology/Ontology.hpp>
#include "Replay.hpp"


// Replay related includes
#include  <map>
#include "posix-low-level-io-helper.h"


// Replay related globals
int issued = 3;	// 3 will be first issued by this replayer
std::map<int,int> fds;
std::map<int,FILE> streams;
std::map<int,int> activityHashTable_int;
std::map<int,int> activityHashTable_network_int;

// Replay related lookup tables and frequently used data

/* string constants for initial ucaid mapping */
std::string str_open ("open");
std::string str_creat ("creat");
std::string str_open64 ("open64");
std::string str_creat64 ("creat64");
std::string str_close ("close");
std::string str_dup ("dup");
std::string str_dup2 ("dup2");
std::string str_dup3 ("dup3");
std::string str_sendfile ("sendfile");
std::string str_write ("write");
std::string str_read ("read");
std::string str_writev ("writev");
std::string str_readv ("readv");
std::string str_pwrite ("pwrite");
std::string str_pread ("pread");
std::string str_pwrite64 ("pwrite64");
std::string str_pread64 ("pread64");
std::string str_pwritev ("pwritev");
std::string str_preadv ("preadv");
std::string str_pwritev64 ("pwritev64");
std::string str_preadv64 ("preadv64");
std::string str_sync ("sync");
std::string str_fsync ("fsync");
std::string str_fdatasync ("fdatasync");
std::string str_lseek ("lseek");
std::string str_posix_fadvise ("posix_fadvise");
std::string str_remove ("remove");
std::string str_rename ("rename");
std::string str___xstat64 ("__xstat64");
std::string str___lxstat64 ("__lxstat64");
std::string str___fxstat64 ("__fxstat64");
std::string str___fxstat ("__fxstat");
std::string str_mmap ("mmap");
std::string str_mmap64 ("mmap64");
std::string str_fopen ("fopen");
std::string str_fopen64 ("fopen64");
std::string str_fdopen ("fdopen");
std::string str_fileno ("fileno");
std::string str_freopen ("freopen");
std::string str_tmpfile ("tmpfile");
std::string str_fclose ("fclose");
std::string str_fflush ("fflush");
std::string str_fgetc ("fgetc");
std::string str_getc ("getc");
std::string str_fputc ("fputc");
std::string str_putc ("putc");
std::string str_fgets ("fgets");
std::string str_fputs ("fputs");
std::string str_fread ("fread");
std::string str_fwrite ("fwrite");
std::string str_fseeko ("fseeko");
std::string str_fseek ("fseek");
std::string str_setbuf ("setbuf");
std::string str_setvbuf ("setvbuf");
std::string str_unlink ("unlink");
std::string str_vfprintf ("vfprintf");
std::string str_vfscanf ("vfscanf");
std::string str_fscanf ("fscanf");
std::string str_fprintf ("fprintf");
std::string str_aio_read ("aio_read");
std::string str_aio_write ("aio_write");
std::string str_lio_listio ("lio_listio");
std::string str_aio_suspend ("aio_suspend");
std::string str_aio_cancel ("aio_cancel");
std::string str_fork ("fork");
std::string str_lockf ("lockf");
std::string str_flock ("flock");
std::string str_socket ("socket");
std::string str_setsockopt ("setsockopt");
std::string str_pipe ("pipe");
std::string str_pipe2 ("pipe2");
std::string str_socketpair ("socketpair");
std::string str_accept ("accept");
std::string str_accept4 ("accept4");

/* fixed map for giant switch */
int posix_open = -1;
int posix_creat = -1;
int posix_open64 = -1;
int posix_creat64 = -1;
int posix_close = -1;
int posix_dup = -1;
int posix_dup2 = -1;
int posix_dup3 = -1;
int posix_sendfile = -1;
int posix_write = -1;
int posix_read = -1;
int posix_writev = -1;
int posix_readv = -1;
int posix_pwrite = -1;
int posix_pread = -1;
int posix_pwrite64 = -1;
int posix_pread64 = -1;
int posix_pwritev = -1;
int posix_preadv = -1;
int posix_pwritev64 = -1;
int posix_preadv64 = -1;
int posix_sync = -1;
int posix_fsync = -1;
int posix_fdatasync = -1;
int posix_lseek = -1;
int posix_posix_fadvise = -1;
int posix_remove = -1;
int posix_rename = -1;
int posix___xstat64 = -1;
int posix___lxstat64 = -1;
int posix___fxstat64 = -1;
int posix___fxstat = -1;
int posix_mmap = -1;
int posix_mmap64 = -1;
int posix_fopen = -1;
int posix_fopen64 = -1;
int posix_fdopen = -1;
int posix_fileno = -1;
int posix_freopen = -1;
int posix_tmpfile = -1;
int posix_fclose = -1;
int posix_fflush = -1;
int posix_fgetc = -1;
int posix_getc = -1;
int posix_fputc = -1;
int posix_putc = -1;
int posix_fgets = -1;
int posix_fputs = -1;
int posix_fread = -1;
int posix_fwrite = -1;
int posix_fseeko = -1;
int posix_fseek = -1;
int posix_setbuf = -1;
int posix_setvbuf = -1;
int posix_unlink = -1;
int posix_vfprintf = -1;
int posix_vfscanf = -1;
int posix_fscanf = -1;
int posix_fprintf = -1;
int posix_aio_read = -1;
int posix_aio_write = -1;
int posix_lio_listio = -1;
int posix_aio_suspend = -1;
int posix_aio_cancel = -1;
int posix_fork = -1;
int posix_lockf = -1;
int posix_flock = -1;
int posix_socket = -1;
int posix_setsockopt = -1;
int posix_pipe = -1;
int posix_pipe2 = -1;
int posix_socketpair = -1;
int posix_accept = -1;
int posix_accept4 = -1;



// Replay related helpers
/**
 * Special helper function to be used by write and reads, which require 
 * buffers of certain size. The function guarentees to provide a address to
 * a buffer of sufficient size.
 */
static char * shared_byte_buffer(unsigned int size){
	static char * buffer = NULL;
	static size_t buffer_size = 0;

	if (buffer_size >= size){
		return buffer;
	}

	// allocate memory for data to write
	if ((buffer = (char *)realloc(buffer, size))) {
		size_t i;
		for (i=buffer_size; i < size; i++){
			buffer[i] = '0';
		}

		buffer_size = size;

		printf("Memory reallocated for %lld bytes\n", (long long int) size);
		return buffer;
	}
	else {
		printf("Not enough memory available to allocate %lld bytes!\n",  (long long int) size);
		exit(1);
	}
}


void dump_map() {
	printf("dump_map()\n");
	for (auto it=activityHashTable_int.begin(); it!=activityHashTable_int.end(); ++it) {
        std::cout << " dm: " << it->first << " => " << it->second << '\n';
	}
	printf("dump_map() end \n");
}



/**
 * set posix_* to cuid
 *
 */
void set_posix_map( string activity_name, int ucaid ) {
	

	std::stringstream ss;
	ss << "set_posix_map:" << activity_name << " <=> " << ucaid;
	std::cout << ss.str() << std::endl;


	//string activity_name = sys_info->lookup_activity_name( activity->ucaid() );  // e.g. fwrite, fread,  read, write...

	// open
	if ( str_open.compare(activity_name) == 0) {
		//BEGIN>>POSIX_open<<END
	} else

	// creat
	if ( str_creat.compare(activity_name) == 0) {
		//BEGIN>>POSIX_creat<<END
	} else

	// open64
	if ( str_open64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_open64<<END
	} else

	// creat64
	if ( str_creat64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_creat64<<END
	} else

	// close
	if ( str_close.compare(activity_name) == 0) {
		//BEGIN>>POSIX_close<<END
	} else

	// dup
	if ( str_dup.compare(activity_name) == 0) {
		//BEGIN>>POSIX_dup<<END
	} else

	// dup2
	if ( str_dup2.compare(activity_name) == 0) {
		//BEGIN>>POSIX_dup2<<END
	} else

	// dup3
	if ( str_dup3.compare(activity_name) == 0) {
		//BEGIN>>POSIX_dup3<<END
	} else

	// sendfile
	if ( str_sendfile.compare(activity_name) == 0) {
		//BEGIN>>POSIX_sendfile<<END
	} else

	// write
	if ( str_write.compare(activity_name) == 0) {
		//BEGIN>>POSIX_write<<END
	} else

	// read
	if ( str_read.compare(activity_name) == 0) {
		//BEGIN>>POSIX_read<<END
	} else

	// writev
	if ( str_writev.compare(activity_name) == 0) {
		//BEGIN>>POSIX_writev<<END
	} else

	// readv
	if ( str_readv.compare(activity_name) == 0) {
		//BEGIN>>POSIX_readv<<END
	} else

	// pwrite
	if ( str_pwrite.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pwrite<<END
	} else

	// pread
	if ( str_pread.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pread<<END
	} else

	// pwrite64
	if ( str_pwrite64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pwrite64<<END
	} else

	// pread64
	if ( str_pread64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pread64<<END
	} else

	// pwritev
	if ( str_pwritev.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pwritev<<END
	} else

	// preadv
	if ( str_preadv.compare(activity_name) == 0) {
		//BEGIN>>POSIX_preadv<<END
	} else

	// pwritev64
	if ( str_pwritev64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pwritev64<<END
	} else

	// preadv64
	if ( str_preadv64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_preadv64<<END
	} else

	// sync
	if ( str_sync.compare(activity_name) == 0) {
		//BEGIN>>POSIX_sync<<END
	} else

	// fsync
	if ( str_fsync.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fsync<<END
	} else

	// fdatasync
	if ( str_fdatasync.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fdatasync<<END
	} else

	// lseek
	if ( str_lseek.compare(activity_name) == 0) {
		//BEGIN>>POSIX_lseek<<END
	} else

	// posix_fadvise
	if ( str_posix_fadvise.compare(activity_name) == 0) {
		//BEGIN>>POSIX_posix_fadvise<<END
	} else

	// remove
	if ( str_remove.compare(activity_name) == 0) {
		//BEGIN>>POSIX_remove<<END
	} else

	// rename
	if ( str_rename.compare(activity_name) == 0) {
		//BEGIN>>POSIX_rename<<END
	} else

	// __xstat64
	if ( str___xstat64.compare(activity_name) == 0) {
		//BEGIN>>POSIX___xstat64<<END
	} else

	// __lxstat64
	if ( str___lxstat64.compare(activity_name) == 0) {
		//BEGIN>>POSIX___lxstat64<<END
	} else

	// __fxstat64
	if ( str___fxstat64.compare(activity_name) == 0) {
		//BEGIN>>POSIX___fxstat64<<END
	} else

	// __fxstat
	if ( str___fxstat.compare(activity_name) == 0) {
		//BEGIN>>POSIX___fxstat<<END
	} else

	// mmap
	if ( str_mmap.compare(activity_name) == 0) {
		//BEGIN>>POSIX_mmap<<END
	} else

	// mmap64
	if ( str_mmap64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_mmap64<<END
	} else

	// fopen
	if ( str_fopen.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fopen<<END
	} else

	// fopen64
	if ( str_fopen64.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fopen64<<END
	} else

	// fdopen
	if ( str_fdopen.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fdopen<<END
	} else

	// fileno
	if ( str_fileno.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fileno<<END
	} else

	// freopen
	if ( str_freopen.compare(activity_name) == 0) {
		//BEGIN>>POSIX_freopen<<END
	} else

	// tmpfile
	if ( str_tmpfile.compare(activity_name) == 0) {
		//BEGIN>>POSIX_tmpfile<<END
	} else

	// fclose
	if ( str_fclose.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fclose<<END
	} else

	// fflush
	if ( str_fflush.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fflush<<END
	} else

	// fgetc
	if ( str_fgetc.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fgetc<<END
	} else

	// getc
	if ( str_getc.compare(activity_name) == 0) {
		//BEGIN>>POSIX_getc<<END
	} else

	// fputc
	if ( str_fputc.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fputc<<END
	} else

	// putc
	if ( str_putc.compare(activity_name) == 0) {
		//BEGIN>>POSIX_putc<<END
	} else

	// fgets
	if ( str_fgets.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fgets<<END
	} else

	// fputs
	if ( str_fputs.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fputs<<END
	} else

	// fread
	if ( str_fread.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fread<<END
	} else

	// fwrite
	if ( str_fwrite.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fwrite<<END
	} else

	// fseeko
	if ( str_fseeko.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fseeko<<END
	} else

	// fseek
	if ( str_fseek.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fseek<<END
	} else

	// setbuf
	if ( str_setbuf.compare(activity_name) == 0) {
		//BEGIN>>POSIX_setbuf<<END
	} else

	// setvbuf
	if ( str_setvbuf.compare(activity_name) == 0) {
		//BEGIN>>POSIX_setvbuf<<END
	} else

	// unlink
	if ( str_unlink.compare(activity_name) == 0) {
		//BEGIN>>POSIX_unlink<<END
	} else

	// vfprintf
	if ( str_vfprintf.compare(activity_name) == 0) {
		//BEGIN>>POSIX_vfprintf<<END
	} else

	// vfscanf
	if ( str_vfscanf.compare(activity_name) == 0) {
		//BEGIN>>POSIX_vfscanf<<END
	} else

	// fscanf
	if ( str_fscanf.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fscanf<<END
	} else

	// fprintf
	if ( str_fprintf.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fprintf<<END
	} else

	// aio_read
	if ( str_aio_read.compare(activity_name) == 0) {
		//BEGIN>>POSIX_aio_read<<END
	} else

	// aio_write
	if ( str_aio_write.compare(activity_name) == 0) {
		//BEGIN>>POSIX_aio_write<<END
	} else

	// lio_listio
	if ( str_lio_listio.compare(activity_name) == 0) {
		//BEGIN>>POSIX_lio_listio<<END
	} else

	// aio_suspend
	if ( str_aio_suspend.compare(activity_name) == 0) {
		//BEGIN>>POSIX_aio_suspend<<END
	} else

	// aio_cancel
	if ( str_aio_cancel.compare(activity_name) == 0) {
		//BEGIN>>POSIX_aio_cancel<<END
	} else

	// fork
	if ( str_fork.compare(activity_name) == 0) {
		//BEGIN>>POSIX_fork<<END
	} else

	// lockf
	if ( str_lockf.compare(activity_name) == 0) {
		//BEGIN>>POSIX_lockf<<END
	} else

	// flock
	if ( str_flock.compare(activity_name) == 0) {
		//BEGIN>>POSIX_flock<<END
	} else

	// socket
	if ( str_socket.compare(activity_name) == 0) {
		//BEGIN>>POSIX_socket<<END
	} else

	// setsockopt
	if ( str_setsockopt.compare(activity_name) == 0) {
		//BEGIN>>POSIX_setsockopt<<END
	} else

	// pipe
	if ( str_pipe.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pipe<<END
	} else

	// pipe2
	if ( str_pipe2.compare(activity_name) == 0) {
		//BEGIN>>POSIX_pipe2<<END
	} else

	// socketpair
	if ( str_socketpair.compare(activity_name) == 0) {
		//BEGIN>>POSIX_socketpair<<END
	} else

	// accept
	if ( str_accept.compare(activity_name) == 0) {
		//BEGIN>>POSIX_accept<<END
	} else

	// accept4
	if ( str_accept4.compare(activity_name) == 0) {
		//BEGIN>>POSIX_accept4<<END
	} else
	{
		// no match 
	}
 

}


ComponentOptions* ReplayPlugin::AvailableOptions()
{
	return new ReplayPluginOptions{};
}

void ReplayPlugin::initPlugin(){
	assert(multiplexer);
	multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>(&ReplayPlugin::notify), false);
	sys_info = facade->get_system_information();
	assert(sys_info);
	std::cout << "Time (HH:MM:SS.NANO)\tDuration\tID\tComponent\tActivity(Attributes)\tParents = ReturnCode" << std::endl;

	findUcaidMapping();
}

void ReplayPlugin::notify(const std::shared_ptr<Activity>& activity, int lost){
	ReplayPluginOptions& opts = getOptions<ReplayPluginOptions>();
	switch (opts.verbosity) {
		case 0:
			replayActivity( activity );
			break;
		case 1:
			activity->print();
			break;
		default:
			cout << "Warninig: invalid verbosity level: " << opts.verbosity << endl;
			replayActivity( activity );
	}
}


static inline void strtime( Timestamp t, stringstream & s )
{
	uint64_t ts = t / 1000000000ull;
	uint64_t ns = t - ( ts * 1000000000ull );

	time_t timeIns = ts;
	struct tm * timeStruct = gmtime( & timeIns );

	char buff[200];
	strftime( buff, 199, "%F %T", timeStruct );

	s << buff;
	snprintf( buff, 20, ".%.10lld", ( long long int ) ns );
	s << buff;
}

static inline void strdelta( Timestamp t, stringstream & s )
{
	uint64_t ts = t / 1000000000ull;
	uint64_t ns = t - ( ts * 1000000000ull );
	char buff[200];
	snprintf( buff, 20, "%lld.%.10lld", ( long long int ) ts, ( long long int ) ns );
	s << buff;
}


void ReplayPlugin::strattribute( const Attribute& attribute, std::stringstream& s ) throw( NotFoundError )
{
	OntologyAttributeFull oa = facade->lookup_attribute_by_ID( attribute.id );
	s << oa.domain << "/" << oa.name << "=";
	if( attribute.value.type() == VariableDatatype::Type::STRING){
		s << '"' << attribute.value << '"' ;
	}else{
		s << attribute.value ;
	}
}




void ReplayPlugin::findUcaidMapping() 
{

	// TODO: API: get cuid by by name, would simplefy everything imensely

	std::stringstream ss;

	try {

		//virtual UniqueInterfaceID           lookup_interfaceID( const string & interface, const string & implementation ) const throw( NotFoundError )  = 0;
		UniqueInterfaceID uiid = sys_info->lookup_interfaceID("POSIX", "");

		// stdio	
		posix_open = sys_info->lookup_activityID(uiid, "open"); ss << "open" << " <=> " << posix_open << " <=> " << sys_info->lookup_activity_name( posix_open ) << std::endl;
		posix_read = sys_info->lookup_activityID(uiid, "read"); ss << "read" << " <=> " << posix_read << " <=> " << sys_info->lookup_activity_name( posix_read ) << std::endl;
		posix_write = sys_info->lookup_activityID(uiid, "write"); ss << "write" << " <=> " << posix_write << " <=> " << sys_info->lookup_activity_name( posix_write ) << std::endl;
		posix_close = sys_info->lookup_activityID(uiid, "close"); ss << "close" << " <=> " << posix_close << " <=> " << sys_info->lookup_activity_name( posix_close ) << std::endl;
		posix_lseek = sys_info->lookup_activityID(uiid, "lseek"); ss << "lseek" << " <=> " << posix_lseek << " <=> " << sys_info->lookup_activity_name( posix_lseek ) << std::endl;
	
		// streaming I/O
		posix_fopen = sys_info->lookup_activityID(uiid, "fopen"); ss << "fopen" << " <=> " << posix_fopen << " <=> " << sys_info->lookup_activity_name( posix_fopen ) << std::endl;
		posix_fread = sys_info->lookup_activityID(uiid, "fread"); ss << "fread" << " <=> " << posix_fread << " <=> " << sys_info->lookup_activity_name( posix_fread ) << std::endl;
		posix_fwrite = sys_info->lookup_activityID(uiid, "fwrite"); ss << "fwrite" << " <=> " << posix_fwrite << " <=> " << sys_info->lookup_activity_name( posix_fwrite ) << std::endl;
		posix_fclose = sys_info->lookup_activityID(uiid, "fclose"); ss << "fclose" << " <=> " << posix_fclose << " <=> " << sys_info->lookup_activity_name( posix_fclose ) << std::endl;
		posix_fseek = sys_info->lookup_activityID(uiid, "fseek"); ss << "fseek" << " <=> " << posix_fseek << " <=> " << sys_info->lookup_activity_name( posix_fseek ) << std::endl;
		

		std::cout << ss.str() << std::endl;;

	} catch( NotFoundError & e ) {	
		cerr << "Interface not found!" << ss.str() << endl;
		//cerr << "Exception" << e << endl;
	}


	//string activity_name = sys_info->lookup_activity_name( activity->ucaid() );  // e.g. fwrite, fread,  read, write...
	//set_posix_map(activity_name, activity->ucaid());

}




void ReplayPlugin::replayActivity( std::shared_ptr<Activity> activity )
{

	printActivity(activity);

	stringstream str;
	try {
		char buff[40];
		siox_time_to_str( activity->time_start(), buff, false );


		activity->time_stop();
		activity->time_start();

		activity->aid();

		UniqueInterfaceID uid = sys_info->lookup_interface_of_activity( activity->ucaid() );

		sys_info->lookup_interface_name( uid );
		sys_info->lookup_interface_implementation( uid );


		// hold activity name to prevent multiple lookups

		int ucaid = activity->ucaid();

		if( ucaid == posix_open ) {
			// ###########################################################
			//wrapped_open(sub_activity->data);
			printf("'- open\n");
			}

		else if( ucaid == posix_creat ) {
			//wrapped_creat(sub_activity->data);
			}

		else if( ucaid == posix_open64 ) {
			//wrapped_open64(sub_activity->data);
			}

		else if( ucaid == posix_creat64 ) {
			//wrapped_creat64(sub_activity->data);
			}

		else if( ucaid == posix_close ) {
			// ###########################################################
			//wrapped_close(sub_activity->data);
			printf("'- close\n");
			}

		else if( ucaid == posix_dup ) {
			//wrapped_dup(sub_activity->data);
			}

		else if( ucaid == posix_dup2 ) {
			//wrapped_dup2(sub_activity->data);
			}

		else if( ucaid == posix_dup3 ) {
			//wrapped_dup3(sub_activity->data);
			}

		else if( ucaid == posix_sendfile ) {
			//wrapped_sendfile(sub_activity->data);
			}

		else if( ucaid == posix_write ) {
			// ###########################################################
			//wrapped_write(sub_activity->data);
			printf("'- write\n");
			}

		else if( ucaid == posix_read ) {
			// ###########################################################
			//wrapped_read(sub_activity->data);
			printf("'- read\n");
			}

		else if( ucaid == posix_writev ) {
			//wrapped_writev(sub_activity->data);
			}

		else if( ucaid == posix_readv ) {
			//wrapped_readv(sub_activity->data);
			}

		else if( ucaid == posix_pwrite ) {
			//wrapped_pwrite(sub_activity->data);
			}

		else if( ucaid == posix_pread ) {
			//wrapped_pread(sub_activity->data);
			}

		else if( ucaid == posix_pwrite64 ) {
			//wrapped_pwrite64(sub_activity->data);
			}

		else if( ucaid == posix_pread64 ) {
			//wrapped_pread64(sub_activity->data);
			}

		else if( ucaid == posix_pwritev ) {
			//wrapped_pwritev(sub_activity->data);
			}

		else if( ucaid == posix_preadv ) {
			//wrapped_preadv(sub_activity->data);
			}

		else if( ucaid == posix_pwritev64 ) {
			//wrapped_pwritev64(sub_activity->data);
			}

		else if( ucaid == posix_preadv64 ) {
			//wrapped_preadv64(sub_activity->data);
			}

		else if( ucaid == posix_sync ) {
			//wrapped_sync(sub_activity->data);
			}

		else if( ucaid == posix_fsync ) {
			//wrapped_fsync(sub_activity->data);
			}

		else if( ucaid == posix_fdatasync ) {
			//wrapped_fdatasync(sub_activity->data);
			}

		else if( ucaid == posix_lseek ) {
			// ###########################################################
			//wrapped_lseek(sub_activity->data);
			printf("'- lseek\n");
			}

		else if( ucaid == posix_posix_fadvise ) {
			//wrapped_posix_fadvise(sub_activity->data);
			}

		else if( ucaid == posix_remove ) {
			//wrapped_remove(sub_activity->data);
			}

		else if( ucaid == posix_rename ) {
			//wrapped_rename(sub_activity->data);
			}

		else if( ucaid == posix___xstat64 ) {
			//wrapped___xstat64(sub_activity->data);
			}

		else if( ucaid == posix___lxstat64 ) {
			//wrapped___lxstat64(sub_activity->data);
			}

		else if( ucaid == posix___fxstat64 ) {
			//wrapped___fxstat64(sub_activity->data);
			}

		else if( ucaid == posix___fxstat ) {
			//wrapped___fxstat(sub_activity->data);
			}

		else if( ucaid == posix_mmap ) {
			//wrapped_mmap(sub_activity->data);
			}

		else if( ucaid == posix_mmap64 ) {
			//wrapped_mmap64(sub_activity->data);
			}

		else if( ucaid == posix_fopen ) {
			// ###########################################################
			//wrapped_fopen(sub_activity->data);
			printf("'- fopen\n");
		
			// TODO: get stream attribute

			//ret = open(d->pathname, flags, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH );
			//activityHashTable_int[d->ret] = ret;
			
			}

		else if( ucaid == posix_fopen64 ) {
			//wrapped_fopen64(sub_activity->data);
			}

		else if( ucaid == posix_fdopen ) {
			//wrapped_fdopen(sub_activity->data);
			}

		else if( ucaid == posix_fileno ) {
			//wrapped_fileno(sub_activity->data);
			}

		else if( ucaid == posix_freopen ) {
			//wrapped_freopen(sub_activity->data);
			}

		else if( ucaid == posix_tmpfile ) {
			//wrapped_tmpfile(sub_activity->data);
			}

		else if( ucaid == posix_fclose ) {
			// ###########################################################
			//wrapped_fclose(sub_activity->data);
			printf("'- fclose\n");
			}

		else if( ucaid == posix_fflush ) {
			//wrapped_fflush(sub_activity->data);
			}

		else if( ucaid == posix_fgetc ) {
			//wrapped_fgetc(sub_activity->data);
			}

		else if( ucaid == posix_getc ) {
			//wrapped_getc(sub_activity->data);
			}

		else if( ucaid == posix_fputc ) {
			//wrapped_fputc(sub_activity->data);
			}

		else if( ucaid == posix_putc ) {
			//wrapped_putc(sub_activity->data);
			}

		else if( ucaid == posix_fgets ) {
			//wrapped_fgets(sub_activity->data);
			}

		else if( ucaid == posix_fputs ) {
			//wrapped_fputs(sub_activity->data);
			}

		else if( ucaid == posix_fread ) {
			// ###########################################################
			//wrapped_fread(sub_activity->data);
			printf("'- fread\n");
			}

		else if( ucaid == posix_fwrite ) {
			// ###########################################################
			//wrapped_fwrite(sub_activity->data);
			printf("'- fwrite\n");
			}

		else if( ucaid == posix_fseeko ) {
			//wrapped_fseeko(sub_activity->data);
			}

		else if( ucaid == posix_fseek ) {
			// ###########################################################
			//wrapped_fseek(sub_activity->data);
			printf("'- fseek\n");
			}

		else if( ucaid == posix_setbuf ) {
			//wrapped_setbuf(sub_activity->data);
			}

		else if( ucaid == posix_setvbuf ) {
			//wrapped_setvbuf(sub_activity->data);
			}

		else if( ucaid == posix_unlink ) {
			//wrapped_unlink(sub_activity->data);
			}

		else if( ucaid == posix_vfprintf ) {
			//wrapped_vfprintf(sub_activity->data);
			}

		else if( ucaid == posix_vfscanf ) {
			//wrapped_vfscanf(sub_activity->data);
			}

		else if( ucaid == posix_fscanf ) {
			//wrapped_fscanf(sub_activity->data);
			}

		else if( ucaid == posix_fprintf ) {
			//wrapped_fprintf(sub_activity->data);
			}

		else if( ucaid == posix_aio_read ) {
			//wrapped_aio_read(sub_activity->data);
			}

		else if( ucaid == posix_aio_write ) {
			//wrapped_aio_write(sub_activity->data);
			}

		else if( ucaid == posix_lio_listio ) {
			//wrapped_lio_listio(sub_activity->data);
			}

		else if( ucaid == posix_aio_suspend ) {
			//wrapped_aio_suspend(sub_activity->data);
			}

		else if( ucaid == posix_aio_cancel ) {
			//wrapped_aio_cancel(sub_activity->data);
			}

		else if( ucaid == posix_fork ) {
			//wrapped_fork(sub_activity->data);
			}

		else if( ucaid == posix_lockf ) {
			//wrapped_lockf(sub_activity->data);
			}

		else if( ucaid == posix_flock ) {
			//wrapped_flock(sub_activity->data);
			}

		else if( ucaid == posix_socket ) {
			//wrapped_socket(sub_activity->data);
			}

		else if( ucaid == posix_setsockopt ) {
			//wrapped_setsockopt(sub_activity->data);
			}

		else if( ucaid == posix_pipe ) {
			//wrapped_pipe(sub_activity->data);
			}

		else if( ucaid == posix_pipe2 ) {
			//wrapped_pipe2(sub_activity->data);
			}

		else if( ucaid == posix_socketpair ) {
			//wrapped_socketpair(sub_activity->data);
			}

		else if( ucaid == posix_accept ) {
			//wrapped_accept(sub_activity->data);
			}

		else if( ucaid == posix_accept4 ) {
			//wrapped_accept4(sub_activity->data);
		}

		else {
        	// not found!
			printf("unknown type - nothing to replay");
		}
        

		/*
		for( auto itr = activity->attributeArray().begin() ; itr != activity->attributeArray().end(); itr++ ) {
			if( itr != activity->attributeArray().begin() ) {
				// new attribute (consider different handling)
			}

			const Attribute& attribute = *itr;
			//strattribute( *itr, str );
			OntologyAttributeFull oa = facade->lookup_attribute_by_ID( attribute.id );
			str << oa.domain << "/" << oa.name << "=";
			if( attribute.value.type() == VariableDatatype::Type::STRING){
				str << '"' << attribute.value << '"' ;
			}elsee
				str << attribute.value ;
			}



		}
		str << ")";
		str << " = " << activity->errorValue();

		if( activity->parentArray().begin() != activity->parentArray().end() ) {
			str << " ";
			for( auto itr = activity->parentArray().begin(); itr != activity->parentArray().end(); itr++ ) {
				if( itr != activity->parentArray().begin() ) {
					str << ", ";
				}
				str << *itr;
			}
		}


		*/

		cout << str.str() << endl;

	} catch( NotFoundError & e ) {
		cerr << "Error while parsing activity! Parsed so far: " << str.str() << endl;
	}

}



void ReplayPlugin::printActivity( std::shared_ptr<Activity> activity )
{
	stringstream str;
	try {
		char buff[40];
		siox_time_to_str( activity->time_start(), buff, false );


		str << "REPLAY:" << " ";

		str << buff << " ";

		strdelta( activity->time_stop() - activity->time_start(), str );
//		if( printHostname )
//			str << " " << sys_info->lookup_node_hostname( a->aid().cid.pid.nid );

		str  << " " << activity->aid() << " ";

		UniqueInterfaceID uid = sys_info->lookup_interface_of_activity( activity->ucaid() );

		str << sys_info->lookup_interface_name( uid ) << " ";
		str << "interface:\"" << sys_info->lookup_interface_implementation( uid ) << "\" ";

		str << sys_info->lookup_activity_name( activity->ucaid() ) << "(";
		for( auto itr = activity->attributeArray().begin() ; itr != activity->attributeArray().end(); itr++ ) {
			if( itr != activity->attributeArray().begin() ) {
				str << ", ";
			}
			strattribute( *itr, str );
		}
		str << ")";
		str << " = " << activity->errorValue();

		if( activity->parentArray().begin() != activity->parentArray().end() ) {
			str << " ";
			for( auto itr = activity->parentArray().begin(); itr != activity->parentArray().end(); itr++ ) {
				if( itr != activity->parentArray().begin() ) {
					str << ", ";
				}
				str << *itr;
			}
		}

		cout << str.str() << endl;


	} catch( NotFoundError & e ) {
		cerr << "Error while parsing activity! Parsed so far: " << str.str() << endl;
	}
}

extern "C" {
void* MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME ()
	{
		return new ReplayPlugin();
	}
}
