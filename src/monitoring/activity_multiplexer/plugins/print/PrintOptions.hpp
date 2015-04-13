/*
 * =====================================================================================
 *
 *       Filename:  PrintOptions.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/03/2015 12:13:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  PrintOptions_INC
#define  PrintOptions_INC

#include <string>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

//@serializable
class PrintPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		unsigned int verbosity;
};
#endif   /* ----- #ifndef PrintOptions_INC  ----- */
