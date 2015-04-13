/*
 * =====================================================================================
 *
 *       Filename:  FileTraceReaderOptions.hpp
 *
 *    Description:   
 *
 *        Version:  1.0
 *        Created:  04/01/2015 04:19:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eugen Betke
 *   Organization:  Uni Hamburg 
 *
 * =====================================================================================
 */


#ifndef  FileAIStreamOptions_INC
#define  FileAIStreamOptions_INC

#include <string>
#include <tools/TraceReader/activity_stream/ActivityInputStreamPluginOptions.hpp>

//@serializable
class FileAIStreamOptions : public tools::ActivityInputStreamPluginOptions {
 public:
	 std::string filename; // contains a sequence of activities
 };

#endif   /* ----- #ifndef FileAIStreamOptions_INC  ----- */
