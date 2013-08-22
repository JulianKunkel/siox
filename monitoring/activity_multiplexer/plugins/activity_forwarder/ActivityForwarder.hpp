
#ifndef ACTIVITYFORWARDER_HPP_Z1AEO5ER
#define ACTIVITYFORWARDER_HPP_Z1AEO5ER

#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

using namespace monitoring;

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ActivityForwarder: public ActivityMultiplexerPlugin, public ActivityMultiplexerListener {
	public:
		/**
		 * Minimal constructor as in and out is necessary
		 *
		 */
		ActivityForwarder( ActivityMultiplexer * in, ActivityMultiplexer * out ) {
			this->in = in;
			this->out = out;

			in->registerListener( dynamic_cast<ActivityMultiplexerListener *>( this ) );
		};

		/**
		 * Implements ActivityMultiplexerListener Notify, passes activity to out.
		 */
		virtual void Notify( Activity * element ) {
			if( out ) {
				out->Log( element );
			}
		}


	private:
		ActivityMultiplexer * in = NULL;
		ActivityMultiplexer * out = NULL;
		/* data */
};

#endif /* end of include guard: ACTIVITYFORWARDER_HPP_Z1AEO5ER */
