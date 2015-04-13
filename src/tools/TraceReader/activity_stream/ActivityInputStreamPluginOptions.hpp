/*
 * =====================================================================================
 *
 *       Filename:  ActivityInputStreamPluginOptions.hpp
 *
 *    Description:  Base options for activity input stream plugins.
 *
 *        Version:  1.0
 *        Created:  04/12/2015 08:41:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eugen Betke 
 *   Organization:  Uni Hamburg 
 *
 * =====================================================================================
 */

#ifndef  ActivityInputStreamPluginOptions_INC
#define  ActivityInputStreamPluginOptions_INC

#include <core/component/component-options.hpp>

namespace tools {

	//@serializable
	class ActivityInputStreamPluginOptions : public core::ComponentOptions {
		// TODO: link to ActivitySerialization when auto loadable
	};

}
#endif   /* ----- #ifndef ActivityInputStreamPluginOptions_INC  ----- */
