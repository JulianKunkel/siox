#include <atomic>

#include <monitoring/activity_builder/ActivityBuilder.hpp>

#include <util/time.h>

using namespace std;


// manage a global unique ID for each thread
static atomic<uint32_t> current_thread_ID( 0 );

namespace monitoring {


	struct InternalRemoteCall : public RemoteCall{
		Activity * activity;
	};


	ActivityBuilder::ActivityBuilder() : thread_id( ++current_thread_ID )
	{
		next_activity_id = 1;
		printf( "ActivityBuilder %p ready.\n", this );
	}

	ActivityBuilder::~ActivityBuilder()
	{
		printf( "ActivityBuilder %p has been shut down.\n", this );
	}

	/**
	 * getNewInstance(): Create a new ActivityBuilder.
	 * @return
	 */
	ActivityBuilder * ActivityBuilder::getNewInstance()
	{
		return new ActivityBuilder();
	}

	/**
	 * getThreadInstance(): Possibly create and return a thread-local ActivityBuilder instance.
	 * @return
	 */
	ActivityBuilder * ActivityBuilder::getThreadInstance()
	{
		static __thread ActivityBuilder * myAB = nullptr;
		if( myAB == nullptr ) {
			myAB = new ActivityBuilder();
		}
		return myAB;
	}

	Activity * ActivityBuilder::startActivity( const ComponentID & cid, UniqueComponentActivityID ucaid, const Timestamp * t )
	{
		Activity * a = new Activity();

		//printf( "ActivityBuilder %p start %p.\n", this, a);

		assert( ucaid != 0 );

		a->aid_.id = next_activity_id++;
		a->aid_.cid = cid;
		a->aid_.thread = thread_id;
		a->ucaid_ = ucaid;
		a->remoteInvoker_ = nullptr;
		a->time_stop_ = 0;
		// Initially, we assume successful outcome of an actvity
		// a->errorValue_ = SIOX_ACTIVITY_SUCCESS;

		// Add this activity to the list of currently active activities
		//activities_in_flight[aid] = a;

		// Get last activity in call stack of this thread
		if( ! activity_stack.empty() ) {
			a->parentArray_.push_back( activity_stack.back()->aid_ );
		}
		activity_stack.push_back( a );

		if( t != nullptr ) {
			a->time_start_ = *t;
		} else {
			a->time_start_ = siox_gettime();
		}
		return a;
	}

	Activity * ActivityBuilder::startActivity( const ComponentID & cid, UniqueComponentActivityID ucaid, NodeID caller_node_id, UniqueInterfaceID caller_unique_interface_id, AssociateID caller_associate_id, const Timestamp * t )
	{
		// REMARK: If t == nullptr, then startActivity will draw the current timestamp and all code in this function will count towards the time of the activity. As of now, it will be left this way for the sake of simplicity.

		Activity * a;
		a = startActivity( cid, ucaid, t );
		a->remoteInvoker_ = new RemoteCallIdentifier( caller_node_id, caller_unique_interface_id, caller_associate_id );

		return a;
	}

	void ActivityBuilder::stopActivity( Activity * a, const Timestamp * t )
	{
		assert( a != nullptr );

		if( t != nullptr ) {
			a->time_stop_ = *t;
		} else {
			a->time_stop_ = siox_gettime();
		}

		assert( a == activity_stack.back() );
		activity_stack.pop_back();
	}

	/* endActivity: All data for this activity has been collected.
	 */
	void ActivityBuilder::endActivity( Activity * a )
	{
		assert( a != nullptr );

		//printf( "ActivityBuilder %p end %p.\n", this, a);

		// Remove Activity from in-flight list
		//activities_in_flight.erase(a->aid_.id);
	}

	void ActivityBuilder::setActivityAttribute( Activity * a, const Attribute & attribute )
	{
		assert( a != nullptr );

		a->attributeArray_.push_back( attribute );
	}

	void ActivityBuilder::reportActivityError( Activity * a, ActivityError error )
	{
		assert( a != nullptr );
		assert( a->errorValue_ == 0 );

		a->errorValue_ = error;
	}

	void ActivityBuilder::linkActivities( Activity * child, const ActivityID & parent )
	{
		assert( child != nullptr );

		child->parentArray_.push_back( parent );
	}

	RemoteCall * ActivityBuilder::setupRemoteCall( Activity * a, NodeID target_node_id, UniqueInterfaceID target_unique_interface_id, AssociateID target_associate_id )
	{
		assert( a != nullptr );

		InternalRemoteCall * rc = new InternalRemoteCall;
		rc->target = RemoteCallIdentifier( target_node_id, target_unique_interface_id, target_associate_id );
		rc->activity = a;

		return rc;
	}

	void ActivityBuilder::setRemoteCallAttribute( RemoteCall * rc, const Attribute & attribute )
	{
		assert( rc != nullptr );

		rc->attributes.push_back( attribute );
	}

	/**
	 * startRemoteCall: Indicate that the remote call will be executed.
	 * The reference to this RemoteCall must not be used afterwards.
	 * @param rc: RemoteCall reference obtained from setupRemoteCall().
	 * @param t:
	 */
	void ActivityBuilder::startRemoteCall( RemoteCall* & erc, const Timestamp * t )
	{
		assert( erc != nullptr );

		InternalRemoteCall *& rc = (InternalRemoteCall* &) erc;

		// The Activity to which this RemoteCall belongs
		Activity * a = rc->activity;

		// Set the Activity reference to nullptr - it is only for internal use
		rc->activity = nullptr;

		a->remoteCallsArray_.push_back( *rc );

		// Free the object and set the pointer to nullptr
		delete rc;
		rc = nullptr;
	}


}
