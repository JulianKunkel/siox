#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Activity.hpp>

#include "SequalizerOptions.hpp"

#include <iostream>
#include <string>


using namespace monitoring;

#define ERROR(...) do { cerr << "[Sequalizer @ " << interface << "] " << __VA_ARGS__ << "!\n"; } while(0)

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class Sequalizer: public ActivityMultiplexerPlugin {
	public:
		/**
		 * Dummy implementation for Sequalizer Options.
		 */
		ComponentOptions * AvailableOptions() {
			return new SequalizerOptions();
		}
        void initPlugin() override;
        void finalize() override;

	private:
		// Information about the layer we are to watch
		string interface = "(yet unknown)";
		string implementation = "(yet unknown)";
		SystemInformationGlobalIDManager * sysinfo;
		UniqueInterfaceID uiid;
        OntologyAttributeID fnAttID;    // file name
		/** Receiving ActivityMultiplexer */
		ActivityMultiplexer * out = nullptr;
		const UniqueComponentActivityID lookupUCAID( const string & activity );
		void redirectHandler( const shared_ptr<Activity> & activity, int lost  );
        void readHandler( const shared_ptr<Activity> & activity, int lost );

        const Attribute * findAttributeByID( const shared_ptr<Activity>& activity, OntologyAttributeID oaid );
};

/**
 * Mutator setup:
 * Register this to a already created multiplexer
 * (a ActivityMultiplexerPlugin always also creates a Multiplexer).
 * Create a second ActivityMultiplexer instance that serves as an
 * out going end of the Mutator.
 */
void Sequalizer::initPlugin() {
    // Retrieve interface, implementation and uiid
    SequalizerOptions & options = getOptions<SequalizerOptions>();
    interface = options.interface;
    implementation = options.implementation;
    // retrieve System Information
    sysinfo = facade->get_system_information();
    fnAttID = facade->lookup_attribute_by_name(interface,"descriptor/filename").aID;
    uiid = sysinfo->lookup_interfaceID(interface, implementation);
    // redgister for "open" Activities
    UniqueInterfaceID open_ucaid = lookupUCAID( "open" );
    //UniqueInterfaceID write_at_ucaid = lookupUCAID( "write_at" );
    UniqueInterfaceID write_ucaid = lookupUCAID( "write" );
    UniqueInterfaceID read_ucaid = lookupUCAID( "read" );
    UniqueInterfaceID close_ucaid = lookupUCAID( "close" );
    if ( open_ucaid != 0 &&  write_ucaid != 0 && read_ucaid != 0 && close_ucaid != 0 ){
        multiplexer->registerForUcaid( open_ucaid, this, static_cast<ActivityMultiplexer::Callback>( &Sequalizer::redirectHandler ), false );
//        multiplexer->registerForUcaid( write_at_ucaid, this, static_cast<ActivityMultiplexer::Callback>( &Sequalizer::writeHandler ), false );
        multiplexer->registerForUcaid( write_ucaid, this, static_cast<ActivityMultiplexer::Callback>( &Sequalizer::redirectHandler ), false );
        multiplexer->registerForUcaid( read_ucaid, this, static_cast<ActivityMultiplexer::Callback>( &Sequalizer::readHandler ), false );
        multiplexer->registerForUcaid( close_ucaid, this, static_cast<ActivityMultiplexer::Callback>( &Sequalizer::redirectHandler ), false );
    }
	out = GET_INSTANCE(ActivityMultiplexer, options.target_multiplexer);

}

void Sequalizer::finalize() {
    multiplexer->unregisterCatchall( this, false );
    ActivityMultiplexerPlugin::finalize();
}

/*
 * Find the UCAID for an activity of the interface we are to watch.
 * If no UCAID could be determined, will emit an error message and return 0.
 * Requires the UIID of the interface to be set!
 */
const UniqueComponentActivityID Sequalizer::lookupUCAID( const string & activityName )
{
	UniqueComponentActivityID ucaid = 0;
	try{
		ucaid = sysinfo->lookup_activityID(uiid, activityName);
	}
	catch (NotFoundError)
	{
		ERROR( "Could not find ID for activity type \"" << activityName << "\"" );
	}

	return ucaid;
}


/*
 * Handler, that redirects activities to the next Multiplexer
 */
void Sequalizer::redirectHandler( const shared_ptr<Activity> & activity, int lost ) {
    out->Log( activity );
}

/*
 * Handler for reads
 */
void Sequalizer::readHandler( const shared_ptr<Activity> & activity, int lost ) {
	std::cout << "Sequalizer: File Read" << std::endl;;
    //TODO: decision needs to be made, what the Plugin does on writes. For now just Print the Activity
    activity->print();
}

/**
 * Find the value of the given attribute for the given activity.
 *
 * Search the activity's attributes for one matching the given AID.
 * If found, return its value; otherwise, return @c NULL.
 *
 * @param activity The activity to be investigated
 * @param oaid The OAID of the attribute to look for
 * @return A constant pointer to the attribute in the activity's data, or @c NULL if not found.
 */
 const Attribute * Sequalizer::findAttributeByID( const shared_ptr<Activity> & activity, OntologyAttributeID oaid )
 {
    // OUTPUT( "Looking for attribute " << oaid << " in " << activity->aid() );

    const vector<Attribute> & attributes = activity->attributeArray();
    // OUTPUT( "Found att array of size " << attributes.size() );

    for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
        // OUTPUT( "...id: " << itr->id );
        if( itr->id == oaid )
            return &( *itr );
    }
    return NULL;
 }

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new Sequalizer();
	}
}
