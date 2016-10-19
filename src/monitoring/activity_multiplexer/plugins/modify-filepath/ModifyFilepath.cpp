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
		 * Dummy implementation for ModifyFilepath Options.
		 */
		ComponentOptions * AvailableOptions() {
			return new ModifyFilepathOptions();
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
    // Retrieve interface, implementation and uiid
    ModifyFilepathOptions & options = getOptions<ModifyFilepathOptions>();
    interface = options.interface;
    implementation = options.implementation;
    // retrieve System Information
    sysinfo = facade->get_system_information();
    fnAttID = facade->lookup_attribute_by_name(interface,"descriptor/filename").aID;
    uiid = sysinfo->lookup_interfaceID(interface, implementation);
    // redgister for "open" Activities
    UniqueInterfaceID ucaid = lookupUCAID( "open" );
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


/*UniqueComponentActivityID
 * Create a new survey and initialize its data
 */
void ModifyFilepath::openHandler( const shared_ptr<Activity> & activity, int lost ) {
	std::cout << "ModifyFilepath: File Opened" << std::endl;;
    VariableDatatype * newFilename = new VariableDatatype("hallo.txt");
    activity->replaceAttribute(Attribute(fnAttID, *newFilename));
    const VariableDatatype * attFileName = activity->findAttribute( fnAttID );

    std::cout << "ModifyFilepath: FileName " << attFileName->str() << std::endl;;
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
