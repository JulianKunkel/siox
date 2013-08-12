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


// Create an implementation of the options.
CREATE_SERIALIZEABLE_CLS(AnomalySkeletonOptions)

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class AnomalySkeleton: public ActivityMultiplexerPlugin, public ActivityMultiplexerListenerSync, public AnomalyPlugin {
private:
	SystemInformationGlobalIDManager * sys;
	OntologyAttribute filesize;
public:
	void Notify(Activity * activity)
	{
		cout << "Notified: type: " ;// << sys->activity_name(activity->aid())	<< endl;
		// check for a specific attribute.
		vector<Attribute> attributes = activity->attributeArray();
		for(auto itr = attributes.begin() ; itr != attributes.end(); itr++){
			Attribute & att = *itr;
			cout << "attribute: " << att.id << endl;
			if( att.id == filesize.aID){
					cout << "Filesize: " << att.value << endl;
			}			
		}
	}

	unique_ptr<set<AnomalyPluginObservation>> queryRecentObservations(){
		return unique_ptr<set<AnomalyPluginObservation>>(new set<AnomalyPluginObservation>());
	}


	ComponentOptions * AvailableOptions(){
		return new AnomalySkeletonOptions();
	}

	void init(ActivityMultiplexer & multiplexer){
		AnomalySkeletonOptions & options = getOptions<AnomalySkeletonOptions>();

		assert(dereferenceFacade != nullptr);
		assert(dereferenceFacade->get_system_information() != nullptr);
		sys = dereferenceFacade->get_system_information();
		dereferenceFacade->registerAnomalyPlugin(this);

		try{
			filesize = dereferenceFacade->lookup_attribute_by_name("test", "filesize");

			const OntologyValue & val = dereferenceFacade->lookup_meta_attribute(filesize, "Meta", "Unit");
			cout << "Unit of filesize: " << val << endl;
		}catch(NotFoundError & e){
			// First run, we cannot register because the ontology does not hold filesize.
			cout << e.what() << endl;
			return;
		}

		multiplexer.registerListener(this);
	}
};

PLUGIN(AnomalySkeleton)

