/*
 * =====================================================================================
 *
 *       Filename:  AccessInfoPlotterOptions.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/06/2015 12:00:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  AccessInfoPlotterOptions_INC
#define  AccessInfoPlotterOptions_INC

#include <string>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

//@serializable
class AccessInfoPlotterPluginOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		unsigned int verbosity;
		std::string regex;
};

#endif   /* ----- #ifndef AccessInfoPlotterOptions_INC  ----- */

