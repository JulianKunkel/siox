#include <iostream>
#include <list>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include <knowledge/reasoner/AnomalyPlugin.hpp>


#include "AnomalySkeletonOptions.hpp"

using namespace std;

using namespace core;
using namespace monitoring;
using namespace knowledge;

class AnomalySkeleton;

enum class ActivityToken : uint8_t {
    OPEN,
    CLOSE,
};


// It is important that the first parent class is of type ActivityMultiplexerPlugin
class AnomalySkeleton: public ActivityMultiplexerPlugin, public ActivityMultiplexerListener, public ActivityMultiplexerListenerAsync, public AnomalyPlugin {
	private:
		SystemInformationGlobalIDManager * sys;
		OntologyAttribute filesize;
		OntologyAttribute filesystem;
		OntologyAttribute filename;

		set<AnomalyPluginObservation> * recentObservations = new set<AnomalyPluginObservation>();


		void addObservation( set<AnomalyPluginObservation> & issues, AnomalyPluginObservation & newIssue ) {
			set<AnomalyPluginObservation>::iterator find = issues.find( newIssue );
			// should be the default case.
			if( find == issues.end() ) {
				// append the item
				issues.insert( newIssue );
			} else {
				AnomalyPluginObservation & existingIssue = ( AnomalyPluginObservation & )( *find );
				existingIssue.occurences += newIssue.occurences;
				existingIssue.delta_time_ms += newIssue.delta_time_ms;
			}
		}
	public:
		void Notify( Activity * activity ) {
			cout << "Notified: type: " ;// << sys->activity_name(activity->aid())   << endl;
			// check for a specific attribute.
			vector<Attribute> attributes = activity->attributeArray();
			for( auto itr = attributes.begin() ; itr != attributes.end(); itr++ ) {
				Attribute & att = *itr;
				cout << "attribute: " << att.id << endl;
				//switch(att.id){
				if( filesize.aID == att.id ) {
					cout << "Filesize: " << att.value << endl;
				}
			}
		}

		void NotifyAsync( int lost_count, Activity * activity ) {
		}

		unique_ptr<set<AnomalyPluginObservation>> queryRecentObservations() {
			auto ptr = unique_ptr<set<AnomalyPluginObservation>>( recentObservations );
			recentObservations = new set<AnomalyPluginObservation>();
			return ptr;
		}


		ComponentOptions * AvailableOptions() {
			return new AnomalySkeletonOptions();
		}

		~AnomalySkeleton() {
			multiplexer->unregisterListener( this );
			multiplexer->unregisterAsyncListener( this );		
		}

		void initPlugin(  ) {
			// AnomalySkeletonOptions & options = getOptions<AnomalySkeletonOptions>();

			assert( facade != nullptr );
			assert( facade->get_system_information() != nullptr );
			sys = facade->get_system_information();

			try {
				filesize = facade->lookup_attribute_by_name( "test", "filesize" );
				filename = facade->lookup_attribute_by_name( "test", "filename" );
				filesystem = facade->lookup_attribute_by_name( "test", "filesystem" );

				const OntologyValue & val = facade->lookup_meta_attribute( filesize, "Meta", "Unit" );
				cout << "Unit of filesize: " << val << endl;
			} catch( NotFoundError & e ) {
				// First run, we cannot register because the ontology does not hold filesize.
				cout << e.what() << endl;
				return;
			}

			multiplexer->registerListener( this );
			multiplexer->registerAsyncListener( this );

			facade->registerAnomalyPlugin( this );
		}
};

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new AnomalySkeleton();
	}
}
