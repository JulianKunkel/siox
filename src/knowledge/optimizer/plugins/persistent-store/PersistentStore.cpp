#include <iostream>
#include <unordered_map>

#include <core/reporting/ComponentReportInterface.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <knowledge/optimizer/OptimizerPluginImplementation.hpp>
#include <knowledge/optimizer/Optimizer.hpp>
#include <knowledge/activity_plugin/ActivityPluginDereferencing.hpp>


#include "PersistentStoreOptions.hpp"


using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;

/*
 This class statically reads hints from the configuration file and provides them.
 */
class PersistentStorePlugin:  public Component, public OptimizerInterface, public ComponentReportInterface {
	public:
		ComponentOptions * AvailableOptions() override;

		void init() override;

		OntologyValue optimalParameter( OntologyAttributeID aid ) const throw( NotFoundError ) override {
			throw( NotFoundError() );
		}
		OntologyValue optimalParameterFor( OntologyAttributeID aid, const Activity * activityToStart ) const throw( NotFoundError ) override;

		ComponentReport prepareReport() override;

		void finalize() override;
	private:
		// filename, attribute, value map
		unordered_map<string, unordered_map<OntologyAttributeID, OntologyValue>> hintSet;

		OntologyAttributeID filenameAttribute;
		UniqueComponentActivityID openActivity;

		// statistics about the invocation
		mutable int sucessfullCalls = 0;
		mutable int errorCalls = 0;
};


void PersistentStorePlugin::init(){
	PersistentStoreOptions & o = getOptions<PersistentStoreOptions>();
	auto facade = GET_INSTANCE(ActivityPluginDereferencing, o.dereferenceFacade); 
	SystemInformationGlobalIDManager * sysinfo = facade->get_system_information();
	assert(sysinfo);

	UniqueInterfaceID uiid = sysinfo->lookup_interfaceID(o.openActivity.interface, o.openActivity.implementation);
	assert(uiid != 0);

	openActivity = sysinfo->lookup_activityID(uiid, o.openActivity.activityName);
	assert(openActivity != 0);

	filenameAttribute = facade->lookup_attribute_by_name(o.filenameAttribute.domain, o.filenameAttribute.name).aID;
	assert(filenameAttribute != 0);

	Optimizer * optimizer = GET_INSTANCE(Optimizer, o.optimizer);
	int ret;

	// now build the map with files, attributes and values
	for(auto itr = o.fileHints.begin(); itr != o.fileHints.end(); itr++){
		FileOptions & fo = *itr;

		unordered_map<OntologyAttributeID, OntologyValue> map;
		for(auto i = fo.hints.begin(); i != fo.hints.end(); i++){
			HintOption & ho = *i;
			auto hintAttribute = facade->lookup_attribute_by_name(ho.domain, ho.name);

			assert(map.find(hintAttribute.aID) == map.end() );
			map[hintAttribute.aID] = OntologyValue(hintAttribute.storage_type, ho.value);

			ret = optimizer->registerPlugin( hintAttribute.aID, this );
			if (ret){
				cout << "Registered PersistentStore attribute " << ho.domain << ":" << ho.name << " with value: " << ho.value << endl;
			}
		}

		assert(hintSet.find(fo.filename) == hintSet.end()); 
		hintSet[fo.filename] = map;
	}
}

void PersistentStorePlugin::finalize(){
	PersistentStoreOptions & o = getOptions<PersistentStoreOptions>();	
	Optimizer * optimizer = GET_INSTANCE(Optimizer, o.optimizer);

	for(auto i = hintSet.begin(); i != hintSet.end(); i++ ){
		for (auto atr = i->second.begin(); atr != i->second.end(); atr++ ) {
			optimizer->unregisterPlugin( atr->first, this );
		}
	}
}

ComponentOptions * PersistentStorePlugin::AvailableOptions() {
	return new PersistentStoreOptions();
}

OntologyValue PersistentStorePlugin::optimalParameterFor( OntologyAttributeID aid, const Activity * activityToStart ) const throw( NotFoundError ){

	// should be true, otherwise programming error...
	assert( activityToStart->ucaid() == openActivity ); 
	// determine the filename
	const AttributeValue * fileName = activityToStart->findAttribute(filenameAttribute);
	if ( fileName ){
		// lookup the name
		string name = fileName->str();

		auto fileHints = hintSet.find(name);
		if ( fileHints != hintSet.end()){
			auto hint = fileHints->second.find(aid);
			if (hint != fileHints->second.end()){
				sucessfullCalls++;
				return hint->second;
			}
		}
	}

	errorCalls++;
	throw( NotFoundError() ); 
}


ComponentReport PersistentStorePlugin::prepareReport() {
	ComponentReport result;
	result.addEntry("callsSucessfull", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, VariableDatatype( sucessfullCalls ) ));
	result.addEntry("callsNotFound", ReportEntry( ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, VariableDatatype( errorCalls ) ));
	return result;
}

extern "C" {
	void * KNOWLEDGE_OPTIMIZER_PLUGING_INSTANCIATOR_NAME()
	{
		return new PersistentStorePlugin();
	}
}
