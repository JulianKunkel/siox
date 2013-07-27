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
	OntologyAttribute * filesize;
public:
	void Notify(Activity * activity)
	{ 
		if(filesize == nullptr) // initial state
			return;
		cout << "Notified: type: " ;// << sys->activity_name(activity->aid())	<< endl;
		// check for a specific attribute.
		vector<Attribute> attributes = activity->attributeArray();
		for(auto itr = attributes.begin() ; itr != attributes.end(); itr++){
			Attribute & att = *itr;
			cout << "attribute: " << att.id << endl;
			if( att.id == filesize->aID){
					cout << "Filesize: " << att.value << endl;
			}			
		}
	}

	ComponentOptions * AvailableOptions(){
		return new AnomalySkeletonOptions();
	}

	void init(ActivityMultiplexerPluginOptions * options, ActivityMultiplexer & multiplexer){
		//AnomalySkeletonOptions * o = (AnomalySkeletonOptions*) options;

		assert(this->dereferenceFacade != nullptr);
		assert(this->dereferenceFacade->get_system_information() != nullptr);
		sys = this->dereferenceFacade->get_system_information();

		filesize = dereferenceFacade->lookup_attribute_by_name("test", "filesize");
		// assert(filesize != nullptr); generally true but not for the first run

		multiplexer.registerListener(this);

		delete(options);
	}
};

PLUGIN(AnomalySkeleton)

