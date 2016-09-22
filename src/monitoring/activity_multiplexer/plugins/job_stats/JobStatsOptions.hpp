/*
 * =====================================================================================
 *
 *       Filename:  JobStatsOptions.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2016 03:02:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  JobStatsOptions_INC
#define  JobStatsOptions_INC

#include <string>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

//@serializable
class JobStatsPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		uint32_t verbosity;
		std::string exclude_regex; // exclude files
		int nfile; // track at most n files (most used)
		std::string interface; // mpi or posix
		std::string output;
};

#endif   /* ----- #ifndef JobStatsOptions_INC  ----- */
