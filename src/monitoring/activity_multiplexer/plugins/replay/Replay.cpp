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
std::map<long,FILE*> streams;
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


void dump_fds() {
	printf("dump_fds()\n");
	for (auto it=fds.begin(); it!=fds.end(); ++it) {
        std::cout << " fds: " << it->first << " => " << it->second << '\n';
	}
	printf("dump_fds() end \n");
}

void dump_streams() {
	printf("dump_streams()\n");
	for (auto it=streams.begin(); it!=streams.end(); ++it) {
        std::cout << " streams: " << it->first << " => " << it->second << '\n';
	}
	printf("dump_streams() end \n");
}


static bool convert_attribute_back( OntologyAttribute & oa, const VariableDatatype & val, void * out_value ){
	switch( val.type() ) {
		case VariableDatatype::Type::INT32:
			*((int32_t*) out_value) = val.int32();
			return true;
		case VariableDatatype::Type::UINT32:
			*((uint32_t*) out_value) = val.uint32();
			return true;
		case VariableDatatype::Type::INT64:
			*((int64_t*) out_value) = val.int64();
			return true;
		case VariableDatatype::Type::UINT64:
			*((uint64_t*) out_value) = val.uint64();
			return true;
		case VariableDatatype::Type::FLOAT:
			*((float*) out_value) = val.flt();
			return true;
		case VariableDatatype::Type::DOUBLE:
			*((double*) out_value) = val.dbl();
			return true;
		case VariableDatatype::Type::STRING: {
			*(char**) out_value = strdup(val.str());
			return true;
		}
		case VariableDatatype::Type::INVALID:
		default:
			assert(0 && "tried to optimize for a VariableDatatype of invalid type");
			return false;
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


bool ReplayPlugin::strattribute_compare( const Attribute& attribute, const char* attributeName) throw( NotFoundError )
{
	std::stringstream s;
	std::stringstream debug_string;

	OntologyAttributeFull oa = facade->lookup_attribute_by_ID( attribute.id );
	s << oa.domain << "/" << oa.name;

	//if( attribute.value.type() == VariableDatatype::Type::STRING){
	//	s << '"' << attribute.value << '"' ;
	//}else{
	//	s << attribute.value ;
	//}
	
	debug_string << "TR: strattribute_compare: s=" << s.str() << "\n";
	cout << debug_string.str();

	if ( s.str().compare(attributeName) == 0 ) {
		return true;
	} else {
		return false;
	}
}




const AttributeValue ReplayPlugin::getActivityAttributeValueByName(  std::shared_ptr<Activity> a, const char * domain, const char * name) throw( NotFoundError )
{
	try {
		
		UniqueInterfaceID uid = sys_info->lookup_interface_of_activity( a->ucaid() );

		auto oa = facade->lookup_attribute_by_name( domain, name );

		for( auto itr = a->attributeArray().begin() ; itr != a->attributeArray().end(); itr++ ) {
			if ( itr->id == oa.aID ){
				// activity has attribute, return value
				return itr->value;
			} else {
				// not found in this iteration
			}
		}

		// parent ids!?
//		if( a->parentArray().begin() != a->parentArray().end() ) {
//			str << " ";
//			for( auto itr = a->parentArray().begin(); itr != a->parentArray().end(); itr++ ) {
//				if( itr != a->parentArray().begin() ) {
//					str << ", ";
//				}
//				str << *itr;
//			}
//		}

	} catch( NotFoundError & e ) {
		cerr << "Error while parsing activity! Parsed so far: (ommited by getActivityAttributeValueByName)" << endl;
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

			
			std::stringstream ss;

			// open(POSIX/hints/openFlags=65, POSIX/descriptor/filename="test.tmp", POSIX/descriptor/filehandle=4) = 0 
			int ret = open(
					getActivityAttributeValueByName(activity, "POSIX", "descriptor/filename").str(),
					translateSIOXFlagsToPOSIX( getActivityAttributeValueByName(activity, "POSIX", "hints/openFlags").uint32() ),
					S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH // TODO: supply siox with rights flags converter
				);

			dump_fds();	
			fds[getActivityAttributeValueByName(activity, "POSIX", "descriptor/filehandle").uint32()] = ret;
			dump_fds();	

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

			// close(POSIX/descriptor/filehandle=4) = 0
			int ret = close(
					fds[getActivityAttributeValueByName(activity, "POSIX", "descriptor/filehandle").uint32()]
				);
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
			
			// write(POSIX/quantity/BytesToWrite=3, POSIX/descriptor/filehandle=4, POSIX/data/MemoryAddress=4196057, POSIX/quantity/BytesWritten=3) = 0
			long size = getActivityAttributeValueByName(activity, "POSIX", "quantity/BytesToWrite").uint64();
			int ret = write(
					fds[getActivityAttributeValueByName(activity, "POSIX", "descriptor/filehandle").uint32()],
					shared_byte_buffer( size ),
					size
				);

			// TODO: assert(ret == expected);

			printf("'- write\n");
			}

		else if( ucaid == posix_read ) {
			// ###########################################################
			//wrapped_read(sub_activity->data);
			
			long size = getActivityAttributeValueByName(activity, "POSIX", "quantity/BytesToRead").uint64();
			int ret = read(
					fds[getActivityAttributeValueByName(activity, "POSIX", "descriptor/filehandle").uint32()],
					shared_byte_buffer( size ),
					size
				);

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
			
			// lseek(POSIX/descriptor/filehandle=4, POSIX/file/position=6)
			int ret = lseek(
					fds[getActivityAttributeValueByName(activity, "POSIX", "descriptor/filehandle").uint32()],
					getActivityAttributeValueByName(activity, "POSIX", "file/position").uint64(),
					SEEK_SET
				);

			// TODO: siox convert seeks flags? though siox is always taking the absolute position? => SEEK_SET
			
				

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
		



			// fopen(POSIX/descriptor/filename="test.tmp", POSIX/hints/openFlags=577, POSIX/descriptor/filehandle=4, POSIX/descriptor/FilePointer=32507344) = 0
			char mode[3];
			translatePOSIXFlagsToFILE( mode, getActivityAttributeValueByName(activity, "POSIX", "hints/openFlags").uint32() );

			FILE* ret = fopen(
					getActivityAttributeValueByName(activity, "POSIX", "descriptor/filename").str(),
					mode
				);

			dump_streams();	
			streams[getActivityAttributeValueByName(activity, "POSIX", "descriptor/FilePointer").uint64()] = ret;
			dump_streams();	


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
			


/*
	// write to non existing file
	stream = fopen(file, "w");

	// everything ok? write and close
	if ( !stream ) {
		perror("fopen");
	}

	if ( !fwrite(buf, sizeof(char), 3, stream) ) { perror("fwrite"); }
	if ( !fseek(stream, 6, SEEK_SET) ) { perror("fseek"); }
	if ( !fwrite(buf+3, sizeof(char), 3, stream) ) { perror("fwrite"); }

	//fwrite(buf+3, sizeof(char), 3, stream);
	if (fclose(stream)) {
		perror("fclose");		
	}
*/

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


/*
	// write to non existing file
	stream = fopen(file, "w");

	// everything ok? write and close
	if ( !stream ) {
		perror("fopen");
	}

	if ( !fwrite(buf, sizeof(char), 3, stream) ) { perror("fwrite"); }
	if ( !fseek(stream, 6, SEEK_SET) ) { perror("fseek"); }
	if ( !fwrite(buf+3, sizeof(char), 3, stream) ) { perror("fwrite"); }

	//fwrite(buf+3, sizeof(char), 3, stream);
	if (fclose(stream)) {
		perror("fclose");		
	}
*/

			long size = getActivityAttributeValueByName(activity, "POSIX", "quantity/BytesToRead").uint64();
			int ret = fread(
					shared_byte_buffer( size ),
					sizeof(char),
					size,
					streams[getActivityAttributeValueByName(activity, "POSIX", "descriptor/FilePointer").uint64()]
				);

			}

		else if( ucaid == posix_fwrite ) {
			// ###########################################################
			//wrapped_fwrite(sub_activity->data);
			

/*
	// write to non existing file
	stream = fopen(file, "w");

	// everything ok? write and close
	if ( !stream ) {
		perror("fopen");
	}

	if ( !fwrite(buf, sizeof(char), 3, stream) ) { perror("fwrite"); }
	if ( !fseek(stream, 6, SEEK_SET) ) { perror("fseek"); }
	if ( !fwrite(buf+3, sizeof(char), 3, stream) ) { perror("fwrite"); }

	//fwrite(buf+3, sizeof(char), 3, stream);
	if (fclose(stream)) {
		perror("fclose");		
	}
*/


			// fwrite(POSIX/quantity/BytesToWrite=3, POSIX/descriptor/FilePointer=32507344, POSIX/data/MemoryAddress=4196217, POSIX/quantity/BytesWritten=3) = 0
			long size = getActivityAttributeValueByName(activity, "POSIX", "quantity/BytesToWrite").uint64();
			int ret = fwrite(
					shared_byte_buffer( size ),
					sizeof(char),
					size,
					streams[getActivityAttributeValueByName(activity, "POSIX", "descriptor/FilePointer").uint64()]
				);


			printf("'- fwrite\n");
			}

		else if( ucaid == posix_fseeko ) {
			//wrapped_fseeko(sub_activity->data);
			}

		else if( ucaid == posix_fseek ) {
			// ###########################################################
			//wrapped_fseek(sub_activity->data);
			printf("'- fseek\n");

			// fseek(POSIX/descriptor/FilePointer=11331200, POSIX/file/position=6) = 0
			int ret = fseek(
					streams[getActivityAttributeValueByName(activity, "POSIX", "descriptor/FilePointer").uint64()],
					getActivityAttributeValueByName(activity, "POSIX", "file/position").uint64(),
					SEEK_SET
				);

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
