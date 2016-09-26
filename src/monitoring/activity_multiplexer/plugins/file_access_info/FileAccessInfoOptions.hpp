/*
 * =====================================================================================
 *
 *       Filename:  FileAccessInfoOptions.hpp
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


#ifndef  FileAccessInfoOptions_INC
#define  FileAccessInfoOptions_INC

#include <string>
#include <vector>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

using namespace std;


//@serializable
class FileAccessInfoPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		uint32_t verbosity;
		string exclude_regex; // exclude files
		int file_limit; // track at most n files (most used)
		vector<string> tokens;
		string interface; // mpi or posix
		string output;
};

#endif   /* ----- #ifndef FileAccessInfoOptions_INC  ----- */
