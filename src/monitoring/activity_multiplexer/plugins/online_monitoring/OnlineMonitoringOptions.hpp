/*
 * =====================================================================================
 *
 *       Filename:  OnlineMonitoringOptions.hpp
 *
 *    Description:  This plugin was developed in cooperatin with Ulf Markwardt 
 *    (ulf.markwardt@tu-dresden.de). It investigates an activity trace an creates
 *    summary for each accessed file.
 *
 *        Version:  1.0
 *        Created:  09/20/2016 03:02:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eugen Betke (betke@dkrz.de), Julian Kunkel (julian.kunkel@googlemail.com)
 *   Organization:  Deutsches Klimarechnezentrum
 *
 * =====================================================================================
 */


#ifndef  OnlineMonitoringOptions_INC
#define  OnlineMonitoringOptions_INC

#include <string>
#include <vector>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;


//@serializable
class OnlineMonitoringPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		/**
		 * @brief Not used
		 */
		uint32_t verbosity;
		/**
		 * @brief Not used. 
		 */
		string exclude_regex;
		/**
		 * @brief Limit the number of output. Only N most used files will be printed. 
		 */
		int file_limit;
		/**
		 * @brief Show read/write trace for each file.
		 */
		int enableTrace;
		/**
		 * @brief 
		 */
		int enableSyscallStats;
		/**
		 * @brief ToDo: syscall names, e.g. fprintf, open, write, open64
		 */
		vector<string> tokens;
		/**
		 * @brief POSIX or MPI interface
		 */
		string interface; // mpi or posix
		/**
		 * @brief Output filename
		 */
		string output;

		int tsdb_enabled;
		string tsdb_host;
		string tsdb_port;
		string tsdb_username;
		string tsdb_password;

		int elastic_enabled;
		string elastic_host;
		string elastic_port;
		string elastic_username;
		string elastic_password;
};

#endif   /* ----- #ifndef OnlineMonitoringOptions_INC  ----- */
