/*
 * =====================================================================================
 *
 *       Filename:  ActivityInputStreamPlugin.hpp
 *
 *    Description:  Abstraction from activity source. A sequence of activities can be 
 *    							provide by real applications, activity generators, files, databases,
 *    							... 
 *
 *        Version:  1.0
 *        Created:  03/29/2015 05:42:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eugen Betke
 *   Organization:  Uni Hamburg 
 *
 * =====================================================================================
 */


#ifndef  ActivityInputStreamPlugin_INC
#define  ActivityInputStreamPlugin_INC

#include <memory>
#include <monitoring/datatypes/Activity.hpp>
#include <core/component/Component.hpp>
#include <tools/TraceReader/activity_stream/ActivityInputStreamPluginOptions.hpp>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

namespace tools {

	/**
	 * @brief Classes derived from this interface provide a sequence of activities. The 
	 * sequence ends with a nullptr activity.
	 */
	class ActivityInputStreamPlugin : public Component
	{
		public:
			/**
			 * @brief Get next activity. The last activity is a nullptr.
			 *
			 * @return next activity
			 */
			virtual std::shared_ptr<monitoring::Activity> nextActivity() = 0;
			virtual monitoring::ActivityMultiplexer* getTargetMultiplexer() = 0;
			virtual ~ActivityInputStreamPlugin(){}
	}; /* -----  end of class TraceReader  ----- */

}		/* -----  end of namespace tools  ----- */


#define ACTIVITY_INPUT_STREAM_PLUGIN_INTERFACE "tools_activityinputstream_plugin"

#endif   /* ----- #ifndef ActivityInputStreamPlugin_INC  ----- */
