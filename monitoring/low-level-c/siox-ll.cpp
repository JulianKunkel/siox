#include <monitoring/low-level-c/siox-ll.h>

/**
 * Implementation of the low-level API
 * The C-implementation serves as lightweight wrapper for C++ classes.
 */


HwID lookup_hardware_id(){
	const char * hostname
}

//typedef struct{
//	HwID hw;
//	uint32_t pid;
//	uint32_t time;
// Each process can create a runtime ID locally KNOWING the HwID from the daemon
ProcessID create_proccess_id(HwID id){
	HwID 32 B,  getpid() 32B, gettimeofday(seconds) 32B 
}