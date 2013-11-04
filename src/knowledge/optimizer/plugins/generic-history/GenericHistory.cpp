#include <iostream>
#include <list>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <knowledge/optimizer/OptimizerPluginInterface.hpp>
#include <knowledge/optimizer/Optimizer.hpp>


#include "GenericHistoryOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;


/*
 Some thoughts for an abstract plugin.

 This plugin remembers for every program, user, and filename (extensions) efficient and inefficient operations.
 It can be applied to any layer that supports the semantics of OPEN, ACCESS (i.e. read(), write(), etc.), optional HINTS and CLOSE.

 A configuration file defines the symbols which map to the OPEN etc. semantics and an integer descriptor as ontology identifier to track these.
 The time needed for the ACCESS semantics may depend on the abstract attributes: "size", offset (which defines some sort of regularity), and the set of supported HINTS.
 A hint may be set either on OPEN, during ACCESS.
 File extension, user and program are considered to behave similar as a hint.
 
 */
class GenericHistoryPlugin: public ActivityMultiplexerPlugin, public OptimizerInterface {
	private:
		Optimizer * optimizer;
	public:

	ComponentOptions * AvailableOptions(){
	  return new GenericHistoryOptions();
	}

	void Notify( Activity * activity ) {
		cout << "GenericHistoryPlugin received " << activity << endl;
		/*
		OntologyAttribute & attribute;
		if( ! optimizer->isPluginRegistered( attribute ) ){
			optimizer->registerPlugin(attribute, & this);
		}
		*/
	}

	void initPlugin(){
		GenericHistoryOptions & o = getOptions<GenericHistoryOptions>();
		optimizer = GET_INSTANCE(Optimizer, o.optimizer);

		// assert(this->dereferenceFacade->get_system_information() != nullptr);
		// sys = this->dereferenceFacade->get_system_information();

		// filesize = dereferenceFacade->lookup_attribute_by_name("test", "filesize");
		// assert(filesize != nullptr); generally true but not for the first run.
	}

	virtual OntologyValue optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) {
		return OntologyValue( 42 );
	}
};


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new GenericHistoryPlugin();
	}
}
