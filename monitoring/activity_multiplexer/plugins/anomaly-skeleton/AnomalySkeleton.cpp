#include <iostream>
#include <list>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerListener.hpp>

#include "AnomalySkeletonOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;

// Create an implementation of the options.
CREATE_SERIALIZEABLE_CLS(AnomalySkeletonOptions)

// It is important that the first parent class is of type ActivityMultiplexerPlugin
class AnomalySkeleton: public ActivityMultiplexerPlugin, public ActivityMultiplexerListenerSync {
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

	ComponentOptions * AvailableOptions(){
		return new AnomalySkeletonOptions();
	}

	void init(ActivityMultiplexer & multiplexer){
		AnomalySkeletonOptions & options = getOptions<AnomalySkeletonOptions>();

		assert(this->dereferenceFacade != nullptr);
		assert(this->dereferenceFacade->get_system_information() != nullptr);
		sys = this->dereferenceFacade->get_system_information();

		try{
			filesize = dereferenceFacade->lookup_attribute_by_name("test", "filesize");
		}catch(NotFoundError & e){
			// First run, we cannot register because the ontology does not hold filesize.
			return;
		}

		multiplexer.registerListener(this);
	}
};

PLUGIN(AnomalySkeleton)

