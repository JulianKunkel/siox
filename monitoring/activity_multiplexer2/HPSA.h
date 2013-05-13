//@author Marc Wiedemann

#ifndef __SIOX_HISTORYPUMPSTATISTICSACTIVITIES__
#define __SIOX_HISTORYPUMPSTATISTICSACTIVITIES__

// Db_pump has connections to the anomaly plugins
// Requirement: anomaly from component A
// Only one db_pump in daemon - because multiplexers suggest individual problems
// Relevant are the last n operations with general aggregate 
// and the last 5 layers in channels
// Example: write 10GiB causing maximal network throuput 117MB/s
// Then individual plugins report bad througput.
// Daemon says anomalies are invalid and reports to admin low nic throughput for Layers-Aggregate L1,L2,L3 

// Statistics and activities (information) are temporarily held in history bip buffer before going to TRS, DW and KB

// Functions:
recv_information_cb();
 {
 recv_information-push_to historybuffer();
 }
// A. Iterators over time beginning with point of time up to now
// Use Unix Epoch with 64Bits which counts seconds from 1970 0:00 UTC to exclude year 2038 error of 32Bit epochs.
// Includes local time_stamp (prerequisite synchronized clocks) + information
iterate(t_min);

// B. history interface registeres several callbacks

historypump_cb(); // (to base either TRS, DWH, or KNB - Why postgres? Hybrid database solution
 {
// 1. allows SQL Standard language, therefore allows exchanging with other database and can be later exchanged with other hybrid databases.
// 2. distributed synchronous clustering possible with extensions
// 3. parallel access possible with extensions 
// 4. active developer community and robust system
// 5. easy tuning of configuration and preinstallation in server distributions )

 push-information_to_base();
 }





register (cbs);
{
}




// allow only memory usage to a certain threshold;
history.MaxMemoryPercent (50);



#endif
