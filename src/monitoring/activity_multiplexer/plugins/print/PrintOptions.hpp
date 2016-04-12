#ifndef  PrintOptions_INC
#define  PrintOptions_INC

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

//@serializable
class PrintPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
/**
@brief Level of verbosity

### Verbosity level 0 ###
Short representation of the activities.

Example: 

    14:32:25.442990674 0.0000000131 (((98,33288,6149333),2),12397,1) POSIX  fgets(POSIX/descriptor/FilePointer=16789952, POSIX/data/MemoryAddress=16925024, POSIX/data/count=2048) = 0 (((98,33288,6149333),2),12387,1)
 
### Verbosity level 1 ###

Verbose representation of the activities. All activity members and the corresponding values are printed in STDOUT. The activity members are described in the monitoring::Activity class.

Example:

    Activity = 0x1c91c70
    t_start = 1460032345202782965
    t_stop  = 1460032345202783175
    ActivityID = (((98,33288,6149333),2),2200,1)
    ActivityID.id = 2200
    ActivityID.thread = 1
    ActivityID.ComponentID.num = 2
    ActivityID.ComponentID.ProcessID.NodeID = 98
    ActivityID.ComponentID.ProcessID.pid    = 33288
    ActivityID.ComponentID.ProcessID.time   = 6149333
    Attributes (3 items):
    (0)
    	id    = 9
    	value = 16445216
    (1)
    	id    = 8
    	value = 16446944
    (2)
    	id    = 7
    	value = 2048
    Parents 1
    	(((98,33288,6149333),2),2195,1)
    RemoteCalls (0 items):

*/
		uint32_t verbosity;
};
#endif
