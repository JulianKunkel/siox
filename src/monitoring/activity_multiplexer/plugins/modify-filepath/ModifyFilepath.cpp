#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Activity.hpp>

#include "ModifyFilepathOptions.hpp"

#include <iostream>
#include <string>


using namespace monitoring;

#define ERROR(...) do { cerr << "[Modify Filepath @ " << interface << "] " << __VA_ARGS__ << "!\n"; } while(0)

/**
 * Forward an activity from one ActivityMultiplexer to another.
 *
 */
class ModifyFilepath: public ActivityMultiplexerPlugin {
	public:
		/**
		 * Implements ActivityMultiplexerListener::Notify, passes activity to out.
		 */
		void Notify( const shared_ptr<Activity> & element, int lostCount ) {
			std::cout << "ModifyFilepath: FILEPATH CHANGER!!! was here!" << std::endl;;
		}

		/**
		 * Dummy implementation for ModifyFilepath Options.
		 */
		ComponentOptions * AvailableOptions() {
			return new ModifyFilepathOptions();
		}
        void initPlugin() override;
        void finalize() override;

	private:
		/** Receiving ActivityMultiplexer */
		ActivityMultiplexer * out = nullptr;
		SystemInformationGlobalIDManager * sysinfo;
		// Information about the layer we are to watch
		string interface = "(yet unknown)";
		string implementation = "(yet unknown)";
		UniqueInterfaceID uiid;
        OntologyAttributeID fnAttID;    // file name
		const UniqueComponentActivityID lookupUCAID( const string & activity );
		void openHandler( const shared_ptr<Activity> & activity, int lost  );
        const Attribute * findAttributeByID( const shared_ptr<Activity>& activity, OntologyAttributeID oaid );
};

/**
 * Mutator setup:
 * Register this to a already created multiplexer
 * (a ActivityMultiplexerPlugin always also creates a Multiplexer).
 * Create a second ActivityMultiplexer instance that serves as an
 * out going end of the Mutator.
 */
void ModifyFilepath::initPlugin() {
    printf("HERE\n");
    ModifyFilepathOptions & options = getOptions<ModifyFilepathOptions>();
    sysinfo = facade->get_system_information();
//			assert(sysinfo);
    // Retrieve interface, implementation and uiid
    interface = options.interface;
    implementation = options.implementation;
    std::cout << "ModifyFilepath: init x" << interface << "x x" << implementation << "x" <<  std::endl;;
    fnAttID = facade->lookup_attribute_by_name(interface,"descriptor/filename").aID;
    uiid = sysinfo->lookup_interfaceID(interface, implementation);
    printf("HERE %d \n", uiid);
    UniqueInterfaceID ucaid = lookupUCAID( "open" );
    printf("HERE ucaid%d \n", ucaid);
    if ( ucaid != 0 ){
        multiplexer->registerForUcaid( ucaid, this, static_cast<ActivityMultiplexer::Callback>( &ModifyFilepath::openHandler ), false );
    }
}

void ModifyFilepath::finalize() {
    multiplexer->unregisterCatchall( this, false );
    ActivityMultiplexerPlugin::finalize();
}

/*
 * Find the UCAID for an activity of the interface we are to watch.
 * If no UCAID could be determined, will emit an error message and return 0.
 * Requires the UIID of the interface to be set!
 */
const UniqueComponentActivityID ModifyFilepath::lookupUCAID( const string & activityName )
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
 * Create a new survey and initialize its data
 */
void ModifyFilepath::openHandler( const shared_ptr<Activity> & activity, int lost ) {
	std::cout << "ModifyFilepath: File Opened" << std::endl;;
    VariableDatatype * newFilename = new VariableDatatype("hallo.txt");
    const Attribute * modified = new Attribute(fnAttID, *newFilename);
    vector<Attribute> & attributes = activity->attributeArray_;
    for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
        // OUTPUT( "...id: " << itr->id );
        if( itr->id == fnAttID )
           *itr = *modified;
    }
    const Attribute * attFileName = findAttributeByID( activity, fnAttID );

    std::cout << "ModifyFilepath: FileName " << attFileName->value.str() << std::endl;;
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
 const Attribute * ModifyFilepath::findAttributeByID( const shared_ptr<Activity> & activity, OntologyAttributeID oaid )
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
		return new ModifyFilepath();
	}
}
