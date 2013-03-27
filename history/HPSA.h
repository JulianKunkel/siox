
#ifndef HPSA_HistoryPumpStatisticsActivities
#define HPSA_HistoryPumpStatisticsActivities


// Statistics and activities (information) are temporarily held in history bip buffer before going to TRS, DW and KB

// Functions:
recv_information_cb();
 {
 recv_information-push_to historybuffer();
 }
// A. Iterators over time beginning with point of time up to now
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
