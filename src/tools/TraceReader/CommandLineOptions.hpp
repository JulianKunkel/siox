/*
 * =====================================================================================
 *
 *       Filename:  CommandLineOptionsPlugin.hpp
 *
 *    Description:  Add command line options support to the derived class.
 *
 *        Version:  1.0
 *        Created:  04/06/2015 08:35:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eugen Betke
 *   Organization:  Uni Hamburg
 *
 * =====================================================================================
 */

#ifndef  CommandLineOptionsPlugin_INC
#define  CommandLineOptionsPlugin_INC

#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>
#include <boost/program_options/options_description.hpp> 
#include <boost/program_options/variables_map.hpp>       


/**
 * @brief Provide supported options to the caller. Caller can use this options to 
 * reconfigure the object.
 */
class CommandLineOptions {
	public:
		/**
		 * @brief This function appends supported options to the passed options description.
		 *
		 * @param od boost options description
		 */
		virtual void moduleOptions(boost::program_options::options_description & od) = 0;

		/**
		 * @brief Set options.
		 *
		 * @param vm boost variable map
		 */
		virtual void setOptions(const boost::program_options::variables_map& vm) = 0;
};

#endif   /* ----- #ifndef CommandLineOptionsPlugin_INC  ----- */
