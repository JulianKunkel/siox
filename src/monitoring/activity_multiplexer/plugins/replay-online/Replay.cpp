/** * GENERATED PLUGIN! - Modify carefully. May loose changes on regeneration. 
 */

#include <regex>
#include <map>
#include <unordered_map>
#include <iostream>
#include <string>

#include <util/time.h>
#include <monitoring/ontology/Ontology.hpp>
#include "Replay.hpp"


// Replay related includes
#include "posix-ll-io-helper.h"
#include <C/siox-ll.h>          // TODO: implementation provided by siox-ll-subset.cpp


#define SUB_CAST_SIOX_STORAGE_32_BIT_INTEGER int32
#define SUB_CAST_SIOX_STORAGE_32_BIT_UINTEGER uint32
#define SUB_CAST_SIOX_STORAGE_64_BIT_INTEGER int64
#define SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER uint64
#define SUB_CAST_SIOX_STORAGE_STRING str

// ucaid mapping
static int posix_open = -1;
static int posix_creat = -1;
static int posix_open64 = -1;
static int posix_creat64 = -1;
static int posix_close = -1;
static int posix_dup = -1;
static int posix_dup2 = -1;
static int posix_dup3 = -1;
static int posix_sendfile = -1;
static int posix_write = -1;
static int posix_read = -1;
static int posix_writev = -1;
static int posix_readv = -1;
static int posix_pwrite = -1;
static int posix_pread = -1;
static int posix_pwrite64 = -1;
static int posix_pread64 = -1;
static int posix_pwritev = -1;
static int posix_preadv = -1;
static int posix_pwritev64 = -1;
static int posix_preadv64 = -1;
static int posix_sync = -1;
static int posix_fsync = -1;
static int posix_fdatasync = -1;
static int posix_lseek = -1;
static int posix_posix_fadvise = -1;
static int posix_remove = -1;
static int posix_rename = -1;
static int posix___xstat64 = -1;
static int posix___lxstat64 = -1;
static int posix___fxstat64 = -1;
static int posix___fxstat = -1;
static int posix_mmap = -1;
static int posix_mmap64 = -1;
static int posix_fopen = -1;
static int posix_fopen64 = -1;
static int posix_fdopen = -1;
static int posix_fileno = -1;
static int posix_freopen = -1;
static int posix_tmpfile = -1;
static int posix_fclose = -1;
static int posix_fflush = -1;
static int posix_fgetc = -1;
static int posix_getc = -1;
static int posix_fputc = -1;
static int posix_putc = -1;
static int posix_fgets = -1;
static int posix_fputs = -1;
static int posix_fread = -1;
static int posix_fwrite = -1;
static int posix_fseeko = -1;
static int posix_fseek = -1;
static int posix_setbuf = -1;
static int posix_setvbuf = -1;
static int posix_unlink = -1;
static int posix_vfprintf = -1;
static int posix_vfscanf = -1;
static int posix_fscanf = -1;
static int posix_fprintf = -1;
static int posix_aio_read = -1;
static int posix_aio_write = -1;
static int posix_lio_listio = -1;
static int posix_aio_suspend = -1;
static int posix_aio_cancel = -1;
static int posix_fork = -1;
static int posix_lockf = -1;
static int posix_flock = -1;
static int posix_socket = -1;
static int posix_setsockopt = -1;
static int posix_pipe = -1;
static int posix_pipe2 = -1;
static int posix_socketpair = -1;
static int posix_accept = -1;
static int posix_accept4 = -1;

// attribute mapping
static OntologyAttribute oa_dataChar;
static OntologyAttribute oa_dataCount;
static OntologyAttribute oa_memoryAddress;
static OntologyAttribute oa_filePointer;
static OntologyAttribute oa_bytesToRead;
static OntologyAttribute oa_bytesToWrite;
static OntologyAttribute oa_filePosition;
static OntologyAttribute oa_fileSeekPosition;
static OntologyAttribute oa_fileExtent;
static OntologyAttribute oa_fileMemoryRegions;
static OntologyAttribute oa_fileOpenFlags;
static OntologyAttribute oa_fileName;
static OntologyAttribute oa_fileSystem;
static OntologyAttribute oa_fileHandle;
static OntologyAttribute oa_bytesWritten;
static OntologyAttribute oa_bytesRead;
static OntologyAttribute oa_fileAdviseExtent;
static OntologyAttribute oa_fileAdvise;
static OntologyAttribute oa_fileBufferSize;
static OntologyAttribute oa_fileBufferMode;

// attribute macros
#define SUB_dataChar "POSIX", "data/character"
#define SUB_TYPE_dataChar SUB_TYPE_SIOX_STORAGE_32_BIT_INTEGER
#define SUB_CAST_dataChar SUB_CAST_SIOX_STORAGE_32_BIT_INTEGER
#define SUB_dataCount "POSIX", "data/count"
#define SUB_TYPE_dataCount SUB_TYPE_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_CAST_dataCount SUB_CAST_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_memoryAddress "POSIX", "data/MemoryAddress"
#define SUB_TYPE_memoryAddress SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_memoryAddress SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_filePointer "POSIX", "descriptor/FilePointer"
#define SUB_TYPE_filePointer SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_filePointer SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_bytesToRead "POSIX", "quantity/BytesToRead"
#define SUB_TYPE_bytesToRead SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_bytesToRead SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_bytesToWrite "POSIX", "quantity/BytesToWrite"
#define SUB_TYPE_bytesToWrite SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_bytesToWrite SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_filePosition "POSIX", "file/position"
#define SUB_TYPE_filePosition SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_filePosition SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_fileSeekPosition "POSIX", "file/seekPosition"
#define SUB_TYPE_fileSeekPosition SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_fileSeekPosition SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_fileExtent "POSIX", "file/extent"
#define SUB_TYPE_fileExtent SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_fileExtent SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_fileMemoryRegions "POSIX", "quantity/memoryRegions"
#define SUB_TYPE_fileMemoryRegions SUB_TYPE_SIOX_STORAGE_32_BIT_INTEGER
#define SUB_CAST_fileMemoryRegions SUB_CAST_SIOX_STORAGE_32_BIT_INTEGER
#define SUB_fileOpenFlags "POSIX", "hints/openFlags"
#define SUB_TYPE_fileOpenFlags SUB_TYPE_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_CAST_fileOpenFlags SUB_CAST_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_fileName "POSIX", "descriptor/filename"
#define SUB_TYPE_fileName SUB_TYPE_SIOX_STORAGE_STRING
#define SUB_CAST_fileName SUB_CAST_SIOX_STORAGE_STRING
#define SUB_fileSystem "Global", "descriptor/filesystem"
#define SUB_TYPE_fileSystem SUB_TYPE_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_CAST_fileSystem SUB_CAST_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_fileHandle "POSIX", "descriptor/filehandle"
#define SUB_TYPE_fileHandle SUB_TYPE_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_CAST_fileHandle SUB_CAST_SIOX_STORAGE_32_BIT_UINTEGER
#define SUB_bytesWritten "POSIX", "quantity/BytesWritten"
#define SUB_TYPE_bytesWritten SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_bytesWritten SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_bytesRead "POSIX", "quantity/BytesRead"
#define SUB_TYPE_bytesRead SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_bytesRead SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_fileAdviseExtent "POSIX", "hint/advise-extent"
#define SUB_TYPE_fileAdviseExtent SUB_TYPE_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_CAST_fileAdviseExtent SUB_CAST_SIOX_STORAGE_64_BIT_UINTEGER
#define SUB_fileAdvise "POSIX", "hints/advise"
#define SUB_TYPE_fileAdvise SUB_TYPE_SIOX_STORAGE_32_BIT_INTEGER
#define SUB_CAST_fileAdvise SUB_CAST_SIOX_STORAGE_32_BIT_INTEGER
#define SUB_fileBufferSize "POSIX", "hints/bufferSize"
#define SUB_TYPE_fileBufferSize SUB_TYPE_SIOX_STORAGE_64_BIT_INTEGER
#define SUB_CAST_fileBufferSize SUB_CAST_SIOX_STORAGE_64_BIT_INTEGER
#define SUB_fileBufferMode "POSIX", "hints/bufferMode"
#define SUB_TYPE_fileBufferMode SUB_TYPE_SIOX_STORAGE_32_BIT_INTEGER
#define SUB_CAST_fileBufferMode SUB_CAST_SIOX_STORAGE_32_BIT_INTEGER


// Replay related globals
int issued = 3;                 // 3 will be first issued by this replayer
std::map < int, int >fds;
std::map < long, FILE * >streams;
std::map < int, int >activityHashTable_int;
std::map < int, int >activityHashTable_network_int;

//////////////////////////////////////////////////////////////////////////////
/// Convert an attribute's value to the generic datatype used in the ontology.
//////////////////////////////////////////////////////////////////////////////
/// @param attribute [in]
/// @param value [in]
//////////////////////////////////////////////////////////////////////////////
/// @return
//////////////////////////////////////////////////////////////////////////////
static VariableDatatype convert_attribute(OntologyAttribute & oa, const void *value)
{
    AttributeValue v;

    switch (oa.storage_type) {
    case (VariableDatatype::Type::UINT32):
        return *((uint32_t *) value);
    case (VariableDatatype::Type::INT32):{
            return *((int32_t *) value);
        }
    case (VariableDatatype::Type::UINT64):
        return *((uint64_t *) value);
    case (VariableDatatype::Type::INT64):{
            return *((int64_t *) value);
        }
    case (VariableDatatype::Type::FLOAT):{
            return *((float *) value);
        }
    case (VariableDatatype::Type::DOUBLE):{
            return *((double *) value);
        }
    case (VariableDatatype::Type::STRING):{
            return (char *) value;
        }
    case (VariableDatatype::Type::INVALID):{
            assert(0);
        }
    }
    return "";
}

static bool convert_attribute_back(OntologyAttribute & oa, const VariableDatatype & val, void *out_value)
{
    switch (val.type()) {
    case VariableDatatype::Type::INT32:
        *((int32_t *) out_value) = val.int32();
        return true;
    case VariableDatatype::Type::UINT32:
        *((uint32_t *) out_value) = val.uint32();
        return true;
    case VariableDatatype::Type::INT64:
        *((int64_t *) out_value) = val.int64();
        return true;
    case VariableDatatype::Type::UINT64:
        *((uint64_t *) out_value) = val.uint64();
        return true;
    case VariableDatatype::Type::FLOAT:
        *((float *) out_value) = val.flt();
        return true;
    case VariableDatatype::Type::DOUBLE:
        *((double *) out_value) = val.dbl();
        return true;
    case VariableDatatype::Type::STRING:{
            *(char **) out_value = strdup(val.str());
            return true;
        }
    case VariableDatatype::Type::INVALID:
    default:
        assert(0 && "tried to optimize for a VariableDatatype of invalid type");
        return false;
    }
}


// Replay related helpers
/**
 * Special helper function to be used by write and reads, which require 
 * buffers of certain size. The function guarentees to provide a address to
 * a buffer of sufficient size.
 */
static char *shared_byte_buffer(unsigned int size)
{
    static char *buffer = NULL;
    static size_t buffer_size = 0;

    if (buffer_size >= size) {
        return buffer;
    }
    // allocate memory for data to write
    if ((buffer = (char *) realloc(buffer, size))) {
        size_t i;
        for (i = buffer_size; i < size; i++) {
            buffer[i] = '0';
        }

        buffer_size = size;

        printf("Memory reallocated for %lld bytes\n", (long long int) size);
        return buffer;
    } else {
        printf("Not enough memory available to allocate %lld bytes!\n", (long long int) size);
        exit(1);
    }
}


void dump_fds()
{
    printf("dump_fds()");
    for (auto it = fds.begin(); it != fds.end(); ++it) {
        std::cout << " fds: " << it->first << " => " << it->second << std::endl;
    }
    printf("dump_fds() end ");
}

void dump_streams()
{
    printf("dump_streams()");
    for (auto it = streams.begin(); it != streams.end(); ++it) {
        std::cout << " streams: " << it->first << " => " << it->second << std::endl;
    }
    printf("dump_streams() end ");
}







ComponentOptions *ReplayPlugin::AvailableOptions()
{
    return new ReplayPluginOptions {
    };
}

void ReplayPlugin::initPlugin()
{
    assert(multiplexer);
    multiplexer->registerCatchall(this, static_cast < ActivityMultiplexer::Callback > (&ReplayPlugin::notify), false);
    sys_info = facade->get_system_information();
    assert(sys_info);
    std::cout << "Time (HH:MM:SS.NANO)\tDuration\tID\tComponent\tActivity(Attributes)\tParents = ReturnCode" << std::
        endl;

    findUcaidMapping();
    findAttributeMapping();
}

void ReplayPlugin::notify(const std::shared_ptr < Activity > &activity, int lost)
{
    ReplayPluginOptions & opts = getOptions < ReplayPluginOptions > ();
    switch (opts.verbosity) {
    case 0:
        replayActivity(activity);
        break;
    case 1:
        activity->print();
        break;
    default:
        cout << "Warninig: invalid verbosity level: " << opts.verbosity << endl;
        replayActivity(activity);
    }
}


static inline void strtime(Timestamp t, stringstream & s)
{
    uint64_t ts = t / 1000000000ull;
    uint64_t ns = t - (ts * 1000000000ull);

    time_t timeIns = ts;
    struct tm *timeStruct = gmtime(&timeIns);

    char buff[200];
    strftime(buff, 199, "%F %T", timeStruct);

    s << buff;
    snprintf(buff, 20, ".%.10lld", (long long int) ns);
    s << buff;
}

static inline void strdelta(Timestamp t, stringstream & s)
{
    uint64_t ts = t / 1000000000ull;
    uint64_t ns = t - (ts * 1000000000ull);
    char buff[200];
    snprintf(buff, 20, "%lld.%.10lld", (long long int) ts, (long long int) ns);
    s << buff;
}


void ReplayPlugin::strattribute(const Attribute & attribute, std::stringstream & s) throw(NotFoundError)
{
    OntologyAttributeFull oa = facade->lookup_attribute_by_ID(attribute.id);
    s << oa.domain << "/" << oa.name << "=";
    if (attribute.value.type() == VariableDatatype::Type::STRING) {
        s << '"' << attribute.value << '"';
    } else {
        s << attribute.value;
    }
}


bool ReplayPlugin::strattribute_compare(const Attribute & attribute, const char *attributeName) throw(NotFoundError)
{
    std::stringstream s;
    std::stringstream debug_string;

    OntologyAttributeFull oa = facade->lookup_attribute_by_ID(attribute.id);
    s << oa.domain << "/" << oa.name;

    //if( attribute.value.type() == VariableDatatype::Type::STRING){
    //  s << '"' << attribute.value << '"' ;
    //}else{
    //  s << attribute.value ;
    //}

    debug_string << "TR: strattribute_compare: s=" << s.str() << "";
    cout << debug_string.str();

    if (s.str().compare(attributeName) == 0) {
        return true;
    } else {
        return false;
    }
}




const AttributeValue ReplayPlugin::getActivityAttributeValueByName(std::shared_ptr < Activity > a, const char *domain,
                                                                   const char *name) throw(NotFoundError)
{
    try {

        UniqueInterfaceID uid = sys_info->lookup_interface_of_activity(a->ucaid());

        auto oa = facade->lookup_attribute_by_name(domain, name);

        for (auto itr = a->attributeArray().begin(); itr != a->attributeArray().end(); itr++) {
            if (itr->id == oa.aID) {
                // activity has attribute, return value
                return itr->value;
            } else {
                // not found in this iteration
            }
        }

    }
    catch(NotFoundError & e) {
        cerr << "Error while parsing activity! Parsed so far: (ommited by getActivityAttributeValueByName)" << endl;
    }
}




void ReplayPlugin::findUcaidMapping()
{
    std::stringstream ss;

    try {

        //virtual UniqueInterfaceID           lookup_interfaceID( const string & interface, const string & implementation ) const throw( NotFoundError )  = 0;
        UniqueInterfaceID uiid = sys_info->lookup_interfaceID("POSIX", "");

        // GENERATED

        // open
        posix_open = sys_info->lookup_activityID(uiid, "open");
        ss << "open" << " <=> " << posix_open << " <=> " << sys_info->lookup_activity_name(posix_open) << std::endl;

        // creat
        posix_creat = sys_info->lookup_activityID(uiid, "creat");
        ss << "creat" << " <=> " << posix_creat << " <=> " << sys_info->lookup_activity_name(posix_creat) << std::endl;

        // open64
        // creat64

        // close
        posix_close = sys_info->lookup_activityID(uiid, "close");
        ss << "close" << " <=> " << posix_close << " <=> " << sys_info->lookup_activity_name(posix_close) << std::endl;

        // dup
        posix_dup = sys_info->lookup_activityID(uiid, "dup");
        ss << "dup" << " <=> " << posix_dup << " <=> " << sys_info->lookup_activity_name(posix_dup) << std::endl;

        // dup2
        posix_dup2 = sys_info->lookup_activityID(uiid, "dup2");
        ss << "dup2" << " <=> " << posix_dup2 << " <=> " << sys_info->lookup_activity_name(posix_dup2) << std::endl;

        // dup3
        posix_dup3 = sys_info->lookup_activityID(uiid, "dup3");
        ss << "dup3" << " <=> " << posix_dup3 << " <=> " << sys_info->lookup_activity_name(posix_dup3) << std::endl;

        // sendfile
        posix_sendfile = sys_info->lookup_activityID(uiid, "sendfile");
        ss << "sendfile" << " <=> " << posix_sendfile << " <=> " << sys_info->
            lookup_activity_name(posix_sendfile) << std::endl;

        // write
        posix_write = sys_info->lookup_activityID(uiid, "write");
        ss << "write" << " <=> " << posix_write << " <=> " << sys_info->lookup_activity_name(posix_write) << std::endl;

        // read
        posix_read = sys_info->lookup_activityID(uiid, "read");
        ss << "read" << " <=> " << posix_read << " <=> " << sys_info->lookup_activity_name(posix_read) << std::endl;

        // writev
        posix_writev = sys_info->lookup_activityID(uiid, "writev");
        ss << "writev" << " <=> " << posix_writev << " <=> " << sys_info->
            lookup_activity_name(posix_writev) << std::endl;

        // readv
        posix_readv = sys_info->lookup_activityID(uiid, "readv");
        ss << "readv" << " <=> " << posix_readv << " <=> " << sys_info->lookup_activity_name(posix_readv) << std::endl;

        // pwrite
        posix_pwrite = sys_info->lookup_activityID(uiid, "pwrite");
        ss << "pwrite" << " <=> " << posix_pwrite << " <=> " << sys_info->
            lookup_activity_name(posix_pwrite) << std::endl;

        // pread
        posix_pread = sys_info->lookup_activityID(uiid, "pread");
        ss << "pread" << " <=> " << posix_pread << " <=> " << sys_info->lookup_activity_name(posix_pread) << std::endl;

        // pwrite64
        // pread64

        // pwritev
        posix_pwritev = sys_info->lookup_activityID(uiid, "pwritev");
        ss << "pwritev" << " <=> " << posix_pwritev << " <=> " << sys_info->
            lookup_activity_name(posix_pwritev) << std::endl;

        // preadv
        posix_preadv = sys_info->lookup_activityID(uiid, "preadv");
        ss << "preadv" << " <=> " << posix_preadv << " <=> " << sys_info->
            lookup_activity_name(posix_preadv) << std::endl;

        // pwritev64
        // preadv64

        // sync
        posix_sync = sys_info->lookup_activityID(uiid, "sync");
        ss << "sync" << " <=> " << posix_sync << " <=> " << sys_info->lookup_activity_name(posix_sync) << std::endl;

        // fsync
        posix_fsync = sys_info->lookup_activityID(uiid, "fsync");
        ss << "fsync" << " <=> " << posix_fsync << " <=> " << sys_info->lookup_activity_name(posix_fsync) << std::endl;

        // fdatasync
        posix_fdatasync = sys_info->lookup_activityID(uiid, "fdatasync");
        ss << "fdatasync" << " <=> " << posix_fdatasync << " <=> " << sys_info->
            lookup_activity_name(posix_fdatasync) << std::endl;

        // lseek
        posix_lseek = sys_info->lookup_activityID(uiid, "lseek");
        ss << "lseek" << " <=> " << posix_lseek << " <=> " << sys_info->lookup_activity_name(posix_lseek) << std::endl;

        // posix_fadvise
        posix_posix_fadvise = sys_info->lookup_activityID(uiid, "posix_fadvise");
        ss << "posix_fadvise" << " <=> " << posix_posix_fadvise << " <=> " << sys_info->
            lookup_activity_name(posix_posix_fadvise) << std::endl;

        // remove
        posix_remove = sys_info->lookup_activityID(uiid, "remove");
        ss << "remove" << " <=> " << posix_remove << " <=> " << sys_info->
            lookup_activity_name(posix_remove) << std::endl;

        // rename
        posix_rename = sys_info->lookup_activityID(uiid, "rename");
        ss << "rename" << " <=> " << posix_rename << " <=> " << sys_info->
            lookup_activity_name(posix_rename) << std::endl;

        // __xstat64
        // __lxstat64
        // __fxstat64
        // __fxstat

        // mmap
        posix_mmap = sys_info->lookup_activityID(uiid, "mmap");
        ss << "mmap" << " <=> " << posix_mmap << " <=> " << sys_info->lookup_activity_name(posix_mmap) << std::endl;

        // mmap64

        // fopen
        posix_fopen = sys_info->lookup_activityID(uiid, "fopen");
        ss << "fopen" << " <=> " << posix_fopen << " <=> " << sys_info->lookup_activity_name(posix_fopen) << std::endl;

        // fopen64

        // fdopen
        posix_fdopen = sys_info->lookup_activityID(uiid, "fdopen");
        ss << "fdopen" << " <=> " << posix_fdopen << " <=> " << sys_info->
            lookup_activity_name(posix_fdopen) << std::endl;

        // fileno
        posix_fileno = sys_info->lookup_activityID(uiid, "fileno");
        ss << "fileno" << " <=> " << posix_fileno << " <=> " << sys_info->
            lookup_activity_name(posix_fileno) << std::endl;

        // freopen
        posix_freopen = sys_info->lookup_activityID(uiid, "freopen");
        ss << "freopen" << " <=> " << posix_freopen << " <=> " << sys_info->
            lookup_activity_name(posix_freopen) << std::endl;

        // tmpfile
        posix_tmpfile = sys_info->lookup_activityID(uiid, "tmpfile");
        ss << "tmpfile" << " <=> " << posix_tmpfile << " <=> " << sys_info->
            lookup_activity_name(posix_tmpfile) << std::endl;

        // fclose
        posix_fclose = sys_info->lookup_activityID(uiid, "fclose");
        ss << "fclose" << " <=> " << posix_fclose << " <=> " << sys_info->
            lookup_activity_name(posix_fclose) << std::endl;

        // fflush
        posix_fflush = sys_info->lookup_activityID(uiid, "fflush");
        ss << "fflush" << " <=> " << posix_fflush << " <=> " << sys_info->
            lookup_activity_name(posix_fflush) << std::endl;

        // fgetc
        posix_fgetc = sys_info->lookup_activityID(uiid, "fgetc");
        ss << "fgetc" << " <=> " << posix_fgetc << " <=> " << sys_info->lookup_activity_name(posix_fgetc) << std::endl;

        // getc
        posix_getc = sys_info->lookup_activityID(uiid, "getc");
        ss << "getc" << " <=> " << posix_getc << " <=> " << sys_info->lookup_activity_name(posix_getc) << std::endl;

        // fputc
        posix_fputc = sys_info->lookup_activityID(uiid, "fputc");
        ss << "fputc" << " <=> " << posix_fputc << " <=> " << sys_info->lookup_activity_name(posix_fputc) << std::endl;

        // putc
        posix_putc = sys_info->lookup_activityID(uiid, "putc");
        ss << "putc" << " <=> " << posix_putc << " <=> " << sys_info->lookup_activity_name(posix_putc) << std::endl;

        // fgets
        posix_fgets = sys_info->lookup_activityID(uiid, "fgets");
        ss << "fgets" << " <=> " << posix_fgets << " <=> " << sys_info->lookup_activity_name(posix_fgets) << std::endl;

        // fputs
        posix_fputs = sys_info->lookup_activityID(uiid, "fputs");
        ss << "fputs" << " <=> " << posix_fputs << " <=> " << sys_info->lookup_activity_name(posix_fputs) << std::endl;

        // fread
        posix_fread = sys_info->lookup_activityID(uiid, "fread");
        ss << "fread" << " <=> " << posix_fread << " <=> " << sys_info->lookup_activity_name(posix_fread) << std::endl;

        // fwrite
        posix_fwrite = sys_info->lookup_activityID(uiid, "fwrite");
        ss << "fwrite" << " <=> " << posix_fwrite << " <=> " << sys_info->
            lookup_activity_name(posix_fwrite) << std::endl;

        // fseeko
        posix_fseeko = sys_info->lookup_activityID(uiid, "fseeko");
        ss << "fseeko" << " <=> " << posix_fseeko << " <=> " << sys_info->
            lookup_activity_name(posix_fseeko) << std::endl;

        // fseek
        posix_fseek = sys_info->lookup_activityID(uiid, "fseek");
        ss << "fseek" << " <=> " << posix_fseek << " <=> " << sys_info->lookup_activity_name(posix_fseek) << std::endl;

        // setbuf
        posix_setbuf = sys_info->lookup_activityID(uiid, "setbuf");
        ss << "setbuf" << " <=> " << posix_setbuf << " <=> " << sys_info->
            lookup_activity_name(posix_setbuf) << std::endl;

        // setvbuf
        posix_setvbuf = sys_info->lookup_activityID(uiid, "setvbuf");
        ss << "setvbuf" << " <=> " << posix_setvbuf << " <=> " << sys_info->
            lookup_activity_name(posix_setvbuf) << std::endl;

        // unlink
        posix_unlink = sys_info->lookup_activityID(uiid, "unlink");
        ss << "unlink" << " <=> " << posix_unlink << " <=> " << sys_info->
            lookup_activity_name(posix_unlink) << std::endl;

        // vfprintf
        posix_vfprintf = sys_info->lookup_activityID(uiid, "vfprintf");
        ss << "vfprintf" << " <=> " << posix_vfprintf << " <=> " << sys_info->
            lookup_activity_name(posix_vfprintf) << std::endl;

        // vfscanf
        posix_vfscanf = sys_info->lookup_activityID(uiid, "vfscanf");
        ss << "vfscanf" << " <=> " << posix_vfscanf << " <=> " << sys_info->
            lookup_activity_name(posix_vfscanf) << std::endl;

        // fscanf
        posix_fscanf = sys_info->lookup_activityID(uiid, "fscanf");
        ss << "fscanf" << " <=> " << posix_fscanf << " <=> " << sys_info->
            lookup_activity_name(posix_fscanf) << std::endl;

        // fprintf
        posix_fprintf = sys_info->lookup_activityID(uiid, "fprintf");
        ss << "fprintf" << " <=> " << posix_fprintf << " <=> " << sys_info->
            lookup_activity_name(posix_fprintf) << std::endl;

        // aio_read
        posix_aio_read = sys_info->lookup_activityID(uiid, "aio_read");
        ss << "aio_read" << " <=> " << posix_aio_read << " <=> " << sys_info->
            lookup_activity_name(posix_aio_read) << std::endl;

        // aio_write
        posix_aio_write = sys_info->lookup_activityID(uiid, "aio_write");
        ss << "aio_write" << " <=> " << posix_aio_write << " <=> " << sys_info->
            lookup_activity_name(posix_aio_write) << std::endl;

        // lio_listio
        posix_lio_listio = sys_info->lookup_activityID(uiid, "lio_listio");
        ss << "lio_listio" << " <=> " << posix_lio_listio << " <=> " << sys_info->
            lookup_activity_name(posix_lio_listio) << std::endl;

        // aio_suspend
        posix_aio_suspend = sys_info->lookup_activityID(uiid, "aio_suspend");
        ss << "aio_suspend" << " <=> " << posix_aio_suspend << " <=> " << sys_info->
            lookup_activity_name(posix_aio_suspend) << std::endl;

        // aio_cancel
        posix_aio_cancel = sys_info->lookup_activityID(uiid, "aio_cancel");
        ss << "aio_cancel" << " <=> " << posix_aio_cancel << " <=> " << sys_info->
            lookup_activity_name(posix_aio_cancel) << std::endl;

        // fork

        // lockf
        posix_lockf = sys_info->lookup_activityID(uiid, "lockf");
        ss << "lockf" << " <=> " << posix_lockf << " <=> " << sys_info->lookup_activity_name(posix_lockf) << std::endl;

        // flock
        posix_flock = sys_info->lookup_activityID(uiid, "flock");
        ss << "flock" << " <=> " << posix_flock << " <=> " << sys_info->lookup_activity_name(posix_flock) << std::endl;

        // socket
        // setsockopt
        // pipe
        // pipe2
        // socketpair
        // accept
        // accept4

        // GENERATED END


		std::cout << ss.str() << std::endl;

    } catch(NotFoundError & e) {
        cerr << "Find Ucaid Mapping: Interface not found!" << ss.str() << endl;
        //cerr << "Exception" << e << endl;
    }
}


void ReplayPlugin::findAttributeMapping()
{
    std::stringstream ss;


    std::cout << "FindAttributeMapping: interface by name";

    //virtual const OntologyAttribute      lookup_attribute_by_name( const string & domain, const string & name ) const throw( NotFoundError ) = 0; 

    try {
        // GENERATED
        oa_dataChar = facade->lookup_attribute_by_name("POSIX", "data/character");
        oa_dataCount = facade->lookup_attribute_by_name("POSIX", "data/count");
        oa_memoryAddress = facade->lookup_attribute_by_name("POSIX", "data/MemoryAddress");
        oa_filePointer = facade->lookup_attribute_by_name("POSIX", "descriptor/FilePointer");
        oa_bytesToRead = facade->lookup_attribute_by_name("POSIX", "quantity/BytesToRead");
        oa_bytesToWrite = facade->lookup_attribute_by_name("POSIX", "quantity/BytesToWrite");
        oa_filePosition = facade->lookup_attribute_by_name("POSIX", "file/position");
        oa_fileSeekPosition = facade->lookup_attribute_by_name("POSIX", "file/seekPosition");
        oa_fileExtent = facade->lookup_attribute_by_name("POSIX", "file/extent");
        oa_fileMemoryRegions = facade->lookup_attribute_by_name("POSIX", "quantity/memoryRegions");
        oa_fileOpenFlags = facade->lookup_attribute_by_name("POSIX", "hints/openFlags");
        oa_fileName = facade->lookup_attribute_by_name("POSIX", "descriptor/filename");
        oa_fileSystem = facade->lookup_attribute_by_name("Global", "descriptor/filesystem");
        oa_fileHandle = facade->lookup_attribute_by_name("POSIX", "descriptor/filehandle");
        oa_bytesWritten = facade->lookup_attribute_by_name("POSIX", "quantity/BytesWritten");
        oa_bytesRead = facade->lookup_attribute_by_name("POSIX", "quantity/BytesRead");
        oa_fileAdviseExtent = facade->lookup_attribute_by_name("POSIX", "hint/advise-extent");
        oa_fileAdvise = facade->lookup_attribute_by_name("POSIX", "hints/advise");
        oa_fileBufferSize = facade->lookup_attribute_by_name("POSIX", "hints/bufferSize");
        oa_fileBufferMode = facade->lookup_attribute_by_name("POSIX", "hints/bufferMode");

        // END GENERATED

    } catch(NotFoundError & e) {
        cerr << "Exception during attribute mapping: Attribute not found!" << ss.str() << endl;
        //cerr << "Exception" << e << endl;
    }
}

/**  * Turn siox-activities into system/library activity. 
 */
void ReplayPlugin::replayActivity(std::shared_ptr < Activity > activity)
{
    stringstream str;

    printActivity(activity);

    try {
        char buff[40];
        siox_time_to_str(activity->time_start(), buff, false);


        activity->time_stop();
        activity->time_start();

        activity->aid();

        UniqueInterfaceID uid = sys_info->lookup_interface_of_activity(activity->ucaid());

        sys_info->lookup_interface_name(uid);
        sys_info->lookup_interface_implementation(uid);


        // prepare some commonly accessed variables
        int ucaid = activity->ucaid();
        if (ucaid == posix_open) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- open\n");

            /* begin */

            int ret;
            char *pathname;
            int flags;
            int translatedFlags = getActivityAttributeValueByName(activity, SUB_fileOpenFlags).SUB_CAST_fileOpenFlags();
			flags = translateSIOXFlagsToPOSIX(translatedFlags);
			ret = open(
				getActivityAttributeValueByName(activity, "POSIX", "descriptor/filename").str(),
				flags,
				S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH // TODO: supply siox with rights flags converter
			);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_creat) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- creat\n");

            /* begin * /

            int ret;
            char *pathname;
            mode_t mode;
            ret = creat(pathname, mode);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_open64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- open64\n");

            /* begin * /

            int ret;
            char *pathname;
            int flags;
            translatedFlags = getActivityAttributeValueByName(activity, SUB_fileOpenFlags).SUB_CAST_fileOpenFlags();
            ret = open64(pathname, flags);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_creat64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- creat64\n");

            /* begin * /

            int ret;
            char *pathname;
            mode_t mode;
            ret = creat64(pathname, mode);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_close) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- close\n");

            /* begin */

            int ret;
            int fd;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = close(fd);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_dup) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- dup\n");

            /* begin * /

            int ret;
            int fd;
            ret = dup(fd);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_dup2) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- dup2\n");

            /* begin * /

            int ret;
            int oldfd;
            int newfd;
            ret = dup2(oldfd, newfd);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_dup3) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- dup3\n");

            /* begin * /

            int ret;
            int oldfd;
            int newfd;
            int flags;
            ret = dup3(oldfd, newfd, flags);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_sendfile) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- sendfile\n");

            /* begin * /

            int ret;
            int out_fd;
            int in_fd;
            off_t *offset;
            size_t count;
            count = getActivityAttributeValueByName(activity, SUB_bytesToWrite).SUB_CAST_bytesToWrite();
            out_fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            in_fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = sendfile(out_fd, in_fd, offset, count);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_write) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- write\n");

            /* begin */

            int ret;
            int fd;
            void *buf;
            size_t count;
            count = getActivityAttributeValueByName(activity, SUB_bytesToWrite).SUB_CAST_bytesToWrite();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
			buf = (void*) getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();

			printf("string to write: %s, count=%d, fd=%d\n", (char*) buf, count, fd);

            ret = write(fd, buf, count);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_read) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- read\n");

            /* begin */

            int ret;
            int fd;
            void *buf;
            size_t count;
            count = getActivityAttributeValueByName(activity, SUB_bytesToRead).SUB_CAST_bytesToRead();
			buf = (void*) getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = read(fd, buf, count);
            Attribute attr(oa_bytesRead.aID, convert_attribute(oa_bytesRead, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_writev) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- writev\n");

            /* begin * /

            int ret;
            int fd;
            struct iovec *iov;
            int iovcnt;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            iovcnt = getActivityAttributeValueByName(activity, SUB_fileMemoryRegions).SUB_CAST_fileMemoryRegions();
            ret = writev(fd, iov, iovcnt);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_readv) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- readv\n");

            /* begin * /

            int ret;
            int fd;
            struct iovec *iov;
            int iovcnt;
            iovcnt = getActivityAttributeValueByName(activity, SUB_fileMemoryRegions).SUB_CAST_fileMemoryRegions();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = readv(fd, iov, iovcnt);
            Attribute attr(oa_bytesRead.aID, convert_attribute(oa_bytesRead, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pwrite) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pwrite\n");

            /* begin * /

            int ret;
            int fd;
            void *buf;
            size_t count;
            off_t offset;
            count = getActivityAttributeValueByName(activity, SUB_bytesToWrite).SUB_CAST_bytesToWrite();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = pwrite(fd, buf, count, offset);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pread) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pread\n");

            /* begin * /

            int ret;
            int fd;
            void *buf;
            size_t count;
            off_t offset;
            count = getActivityAttributeValueByName(activity, SUB_bytesToRead).SUB_CAST_bytesToRead();
            buf = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = pread(fd, buf, count, offset);
            Attribute attr(oa_bytesRead.aID, convert_attribute(oa_bytesRead, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pwrite64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pwrite64\n");

            /* begin * /

            int ret;
            int fd;
            void *buf;
            size_t count;
            off_t offset;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            count = getActivityAttributeValueByName(activity, SUB_bytesToWrite).SUB_CAST_bytesToWrite();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            buf = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = pwrite64(fd, buf, count, offset);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pread64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pread64\n");

            /* begin * /

            int ret;
            int fd;
            void *buf;
            size_t count;
            off_t offset;
            count = getActivityAttributeValueByName(activity, SUB_bytesToRead).SUB_CAST_bytesToRead();
            buf = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = pread64(fd, buf, count, offset);
            Attribute attr(oa_bytesRead.aID, convert_attribute(oa_bytesRead, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pwritev) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pwritev\n");

            /* begin * /

            int ret;
            int fd;
            struct iovec *iov;
            int iovcnt;
            off_t offset;
            iovcnt = getActivityAttributeValueByName(activity, SUB_fileMemoryRegions).SUB_CAST_fileMemoryRegions();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = pwritev(fd, iov, iovcnt, offset);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_preadv) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- preadv\n");

            /* begin * /

            int ret;
            int fd;
            struct iovec *iov;
            int iovcnt;
            off_t offset;
            iovcnt = getActivityAttributeValueByName(activity, SUB_fileMemoryRegions).SUB_CAST_fileMemoryRegions();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = preadv(fd, iov, iovcnt, offset);
            Attribute attr(oa_bytesRead.aID, convert_attribute(oa_bytesRead, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pwritev64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pwritev64\n");

            /* begin * /

            int ret;
            int fd;
            struct iovec *iov;
            int iovcnt;
            off_t offset;
            iovcnt = getActivityAttributeValueByName(activity, SUB_fileMemoryRegions).SUB_CAST_fileMemoryRegions();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = pwritev64(fd, iov, iovcnt, offset);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_preadv64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- preadv64\n");

            /* begin * /

            int ret;
            int fd;
            struct iovec *iov;
            int iovcnt;
            off_t offset;
            iovcnt = getActivityAttributeValueByName(activity, SUB_fileMemoryRegions).SUB_CAST_fileMemoryRegions();
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = preadv64(fd, iov, iovcnt, offset);
            Attribute attr(oa_bytesRead.aID, convert_attribute(oa_bytesRead, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_sync) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- sync\n");

            /* begin * /

            int ret;
            ret = sync();

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fsync) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fsync\n");

            /* begin * /

            int ret;
            int fd;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = fsync(fd);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fdatasync) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fdatasync\n");

            /* begin * /

            int ret;
            int fd;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = fdatasync(fd);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_lseek) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- lseek\n");

            /* begin */

            int ret;
            int fd;
            off_t offset;
            int whence;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            offset = getActivityAttributeValueByName(activity, SUB_fileSeekPosition).SUB_CAST_fileSeekPosition();
			whence = SEEK_SET;

			print("fd=%d, offset=%d, whence=%d, SEEK_SET=%d\n", fd, offset, whence, SEEK_SET);

            ret = lseek(fd, offset, whence);
            Attribute attr(oa_filePosition.aID, convert_attribute(oa_filePosition, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_posix_fadvise) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- posix_fadvise\n");

            /* begin * /

            int ret;
            int fd;
            off_t offset;
            off_t len;
            int advise;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            len = getActivityAttributeValueByName(activity, SUB_fileAdviseExtent).SUB_CAST_fileAdviseExtent();
            advise = getActivityAttributeValueByName(activity, SUB_fileAdvise).SUB_CAST_fileAdvise();
            ret = posix_fadvise(fd, offset, len, advise);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_remove) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- remove\n");

            /* begin * /

            int ret;
            char *filename;
            ret = remove(filename);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_rename) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- rename\n");

            /* begin * /

            int ret;
            char *oldname;
            char *newname;
            ret = rename(oldname, newname);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix___xstat64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- __xstat64\n");

            /* begin * /

            int ret;
            int __ver;
            char *path;
            struct stat64 *buf;
            ret = __xstat64(__ver, path, buf);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix___lxstat64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- __lxstat64\n");

            /* begin * /

            int ret;
            int __ver;
            char *path;
            struct stat64 *buf;
            ret = __lxstat64(__ver, path, buf);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix___fxstat64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- __fxstat64\n");

            /* begin * /

            int ret;
            int __ver;
            int fd;
            struct stat64 *buf;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = __fxstat64(__ver, fd, buf);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix___fxstat) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- __fxstat\n");

            /* begin * /

            int ret;
            int __ver;
            int fd;
            struct stat *buf;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = __fxstat(__ver, fd, buf);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_mmap) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- mmap\n");

            /* begin * /

            int ret;
            void *address;
            size_t length;
            int protect;
            int flags;
            int fd;
            off_t offset;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            length = getActivityAttributeValueByName(activity, SUB_fileExtent).SUB_CAST_fileExtent();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = mmap(address, length, protect, flags, fd, offset);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_mmap64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- mmap64\n");

            /* begin * /

            int ret;
            void *address;
            size_t length;
            int protect;
            int flags;
            int fd;
            off_t offset;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            length = getActivityAttributeValueByName(activity, SUB_fileExtent).SUB_CAST_fileExtent();
            offset = getActivityAttributeValueByName(activity, SUB_filePosition).SUB_CAST_filePosition();
            ret = mmap64(address, length, protect, flags, fd, offset);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fopen) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fopen\n");

            /* begin * /

            int ret;
            char *filename;
            char *mode;
            translatedFlags = getActivityAttributeValueByName(activity, SUB_fileOpenFlags).SUB_CAST_fileOpenFlags();
            ret = fopen(filename, mode);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &fd));
            activity->attributeArray_.push_back(attr);
            Attribute attr(oa_filePointer.aID, convert_attribute(oa_filePointer, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fopen64) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fopen64\n");

            /* begin * /

            int ret;
            char *filename;
            char *mode;
            translatedFlags = getActivityAttributeValueByName(activity, SUB_fileOpenFlags).SUB_CAST_fileOpenFlags();
            ret = fopen64(filename, mode);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &fd));
            activity->attributeArray_.push_back(attr);
            Attribute attr(oa_filePointer.aID, convert_attribute(oa_filePointer, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fdopen) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fdopen\n");

            /* begin * /

            int ret;
            int fd;
            char *mode;
            translatedFlags = getActivityAttributeValueByName(activity, SUB_fileOpenFlags).SUB_CAST_fileOpenFlags();
            ret = fdopen(fd, mode);
            Attribute attr(oa_filePointer.aID, convert_attribute(oa_filePointer, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fileno) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fileno\n");

            /* begin * /

            int ret;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = fileno(stream);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_freopen) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- freopen\n");

            /* begin * /

            int ret;
            char *filename;
            char *mode;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            translatedFlags = getActivityAttributeValueByName(activity, SUB_fileOpenFlags).SUB_CAST_fileOpenFlags();
            ret = freopen(filename, mode, stream);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &fd));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_tmpfile) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- tmpfile\n");

            /* begin * /

            int ret;
            ret = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = tmpfile();
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &fd));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fclose) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fclose\n");

            /* begin * /

            int ret;
            FILE *stream;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = fclose(stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fflush) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fflush\n");

            /* begin * /

            int ret;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = fflush(stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fgetc) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fgetc\n");

            /* begin * /

            int ret;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = fgetc(stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_getc) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- getc\n");

            /* begin * /

            int ret;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = getc(stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fputc) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fputc\n");

            /* begin * /

            int ret;
            int character;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            character = getActivityAttributeValueByName(activity, SUB_dataChar).SUB_CAST_dataChar();
            ret = fputc(character, stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_putc) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- putc\n");

            /* begin * /

            int ret;
            int character;
            FILE *stream;
            character = getActivityAttributeValueByName(activity, SUB_dataChar).SUB_CAST_dataChar();
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = putc(character, stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fgets) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fgets\n");

            /* begin * /

            int ret;
            char *str;
            int num;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            str = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            num = getActivityAttributeValueByName(activity, SUB_dataCount).SUB_CAST_dataCount();
            ret = fgets(str, num, stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fputs) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fputs\n");

            /* begin * /

            int ret;
            char *str;
            FILE *stream;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            str = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = fputs(str, stream);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fread) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fread\n");

            /* begin * /

            int ret;
            void *ptr;
            size_t size;
            size_t count;
            FILE *stream;
            payload = getActivityAttributeValueByName(activity, SUB_bytesToRead).SUB_CAST_bytesToRead();
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ptr = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = fread(ptr, size, count, stream);
            Attribute attr(oa_bytesRead.aID, convert_attribute(oa_bytesRead, &posDelta));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fwrite) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fwrite\n");

            /* begin * /

            int ret;
            void *ptr;
            size_t size;
            size_t count;
            FILE *stream;
            payload = getActivityAttributeValueByName(activity, SUB_bytesToWrite).SUB_CAST_bytesToWrite();
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ptr = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = fwrite(ptr, size, count, stream);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &posDelta));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fseeko) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fseeko\n");

            /* begin * /

            int ret;
            FILE *stream;
            off_t offset;
            int whence;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = fseeko(stream, offset, whence);
            Attribute attr(oa_filePosition.aID, convert_attribute(oa_filePosition, &pos));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fseek) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fseek\n");

            /* begin * /

            int ret;
            FILE *stream;
            long offset;
            int whence;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            ret = fseek(stream, offset, whence);
            Attribute attr(oa_filePosition.aID, convert_attribute(oa_filePosition, &pos));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_setbuf) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- setbuf\n");

            /* begin * /

            int ret;
            FILE *stream;
            char *buffer;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            buffer = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = setbuf(stream, buffer);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_setvbuf) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- setvbuf\n");

            /* begin * /

            int ret;
            FILE *stream;
            char *buffer;
            int mode;
            size_t size;
            size = getActivityAttributeValueByName(activity, SUB_fileBufferSize).SUB_CAST_fileBufferSize();
            mode = getActivityAttributeValueByName(activity, SUB_fileBufferMode).SUB_CAST_fileBufferMode();
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            buffer = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = setvbuf(stream, buffer, mode, size);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_unlink) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- unlink\n");

            /* begin * /

            int ret;
            char *pathname;
            ret = unlink(pathname);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_vfprintf) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- vfprintf\n");

            /* begin * /

            int ret;
            FILE *stream;
            char *format;
            va_list arg;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            format = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = vfprintf(stream, format, arg);
            Attribute attr(oa_bytesWritten.aID, convert_attribute(oa_bytesWritten, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_vfscanf) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- vfscanf\n");

            /* begin * /

            int ret;
            FILE *stream;
            char *format;
            va_list arg;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            format = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = vfscanf(stream, format, arg);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fscanf) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fscanf\n");

            /* begin * /

            int ret;
            FILE *stream;
            char *format;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            format = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = fscanf(stream, format);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fprintf) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fprintf\n");

            /* begin * /

            int ret;
            FILE *stream;
            char *format;
            stream = getActivityAttributeValueByName(activity, SUB_filePointer).SUB_CAST_filePointer();
            format = getActivityAttributeValueByName(activity, SUB_memoryAddress).SUB_CAST_memoryAddress();
            ret = fprintf(stream, format);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_aio_read) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- aio_read\n");

            /* begin * /

            int ret;
            struct aiocb *cb;
            ret = aio_read(cb);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_aio_write) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- aio_write\n");

            /* begin * /

            int ret;
            struct aiocb *cb;
            ret = aio_write(cb);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_lio_listio) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- lio_listio\n");

            /* begin * /

            int ret;
            int mode;
            struct aiocb *aiocb_list;
            int nitems;
            struct sigevent *sevp;
            ret = lio_listio(mode, aiocb_list, nitems, sevp);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_aio_suspend) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- aio_suspend\n");

            /* begin * /

            int ret;
            struct aiocb *aiocb_list;
            int nitems;
            struct timespec *timeout;
            ret = aio_suspend(aiocb_list, nitems, timeout);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_aio_cancel) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- aio_cancel\n");

            /* begin * /

            int ret;
            int fd;
            struct aiocb *aiocbp;
            ret = aio_cancel(fd, aiocbp);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_fork) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- fork\n");

            /* begin * /

            int ret;
            ret = fork();

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_lockf) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- lockf\n");

            /* begin * /

            int ret;
            int fd;
            int cmd;
            off_t len;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = lockf(fd, cmd, len);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_flock) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- flock\n");

            /* begin * /

            int ret;
            int fd;
            int operation;
            fd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = flock(fd, operation);

            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_socket) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- socket\n");

            /* begin * /

            int ret;
            int domain;
            int type;
            int protocol;
            ret = socket(domain, type, protocol);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_setsockopt) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- setsockopt\n");

            /* begin * /

            int ret;
            int sockfd;
            int level;
            int optname;
            void *optval;
            socklen_t optlen;
            ret = setsockopt(sockfd, level, optname, optval, optlen);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &sockfd));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pipe) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pipe\n");

            /* begin * /

            int ret;
            int pipefd;
            ret = pipe(pipefd);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &pipefd[0]));
            activity->attributeArray_.push_back(attr);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &pipefd[1]));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_pipe2) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- pipe2\n");

            /* begin * /

            int ret;
            int pipefd;
            int flags;
            ret = pipe2(pipefd, flags);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &pipefd[0]));
            activity->attributeArray_.push_back(attr);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &pipefd[1]));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_socketpair) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- socketpair\n");

            /* begin * /

            int ret;
            int domain;
            int type;
            int protocol;
            int sv;
            ret = socketpair(domain, type, protocol, sv);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &sv[0]));
            activity->attributeArray_.push_back(attr);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &sv[1]));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_accept) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- accept\n");

            /* begin * /

            int ret;
            int sockfd;
            struct sockaddr *addr;
            socklen_t *addrlen;
            sockfd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = accept(sockfd, addr, addrlen);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else if (ucaid == posix_accept4) {
            // TODO
            // GENERATED FROM TEMPLATE
            printf("'- accept4\n");

            /* begin * /

            int ret;
            int sockfd;
            struct sockaddr *addr;
            socklen_t *addrlen;
            int flags;
            sockfd = getActivityAttributeValueByName(activity, SUB_fileHandle).SUB_CAST_fileHandle();
            ret = accept4(sockfd, addr, addrlen, flags);
            Attribute attr(oa_fileHandle.aID, convert_attribute(oa_fileHandle, &ret));
            activity->attributeArray_.push_back(attr);
            /* end */

            // GENERATED FROM TEMPLATE END

        } else {
            // not found!
            printf("unknown type - nothing to replay");
        }


        cout << str.str() << endl;

    }
    catch(NotFoundError & e) {
        cerr << "Error while parsing activity! Parsed so far: " << str.str() << endl;
    }

    printActivity(activity);
    printf("DONE \n\n\n");

}                               // end of ReplayPlugin::replayActivity()


void ReplayPlugin::printActivity(std::shared_ptr < Activity > activity)
{
    stringstream str;
    try {
        char buff[40];
        siox_time_to_str(activity->time_start(), buff, false);


        str << "REPLAY ONLINE:" << " ";

        str << buff << " ";

        strdelta(activity->time_stop() - activity->time_start(), str);
//    if( printHostname )
//      str << " " << sys_info->lookup_node_hostname( a->aid().cid.pid.nid );

        str << " " << activity->aid() << " ";

        UniqueInterfaceID uid = sys_info->lookup_interface_of_activity(activity->ucaid());

        str << sys_info->lookup_interface_name(uid) << " ";
        str << "interface:\"" << sys_info->lookup_interface_implementation(uid) << "\" ";

        str << sys_info->lookup_activity_name(activity->ucaid()) << "(";
        for (auto itr = activity->attributeArray().begin(); itr != activity->attributeArray().end(); itr++) {
            if (itr != activity->attributeArray().begin()) {
                str << ", ";
            }
            strattribute(*itr, str);
        }
        str << ")";
        str << " = " << activity->errorValue();

        if (activity->parentArray().begin() != activity->parentArray().end()) {
            str << " ";
            for (auto itr = activity->parentArray().begin(); itr != activity->parentArray().end(); itr++) {
                if (itr != activity->parentArray().begin()) {
                    str << ", ";
                }
                str << *itr;
            }
        }

        cout << str.str() << endl;


    }
    catch(NotFoundError & e) {
        cerr << "Error while parsing activity! Parsed so far: " << str.str() << endl;
    }
}


extern "C" {
    void *MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME() {
        return new ReplayPlugin();
}}
