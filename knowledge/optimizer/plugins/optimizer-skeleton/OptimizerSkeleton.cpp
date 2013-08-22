#include <iostream>
#include <list>


#include <monitoring/activity_multiplexer/OptimizerPluginImplementation.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>

#include "OptimizerPluginOptions.hpp"

using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;

// Create an implementation of the options.
CREATE_SERIALIZEABLE_CLS( OptimizerPluginOptions )

// It is important that the first parent class is of type OptimizerPlugin
class OptimizerPluginSkeleton: public OptimizerPlugin {
	private:
		/*  SystemInformationGlobalIDManager * sys;
		    OntologyAttribute * filesize;
		*/
	public:
		/*  void Notify(Activity * activity)
		    {
		        if(filesize == nullptr) // initial state
		            return;
		        cout << "Notified: type: " ;// << sys->activity_name(activity->aid())   << endl;
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
		        return new OptimizerSkeletonOptions();
		    }

		    void init(ActivityMultiplexer & multiplexer){
		        AnomalySkeletonOptions & options = getOptions<AnomalySkeletonOptions>();

		        assert(this->dereferenceFacade != nullptr);
		        assert(this->dereferenceFacade->get_system_information() != nullptr);
		        sys = this->dereferenceFacade->get_system_information();

		        filesize = dereferenceFacade->lookup_attribute_by_name("test", "filesize");
		        // assert(filesize != nullptr); generally true but not for the first run

		        multiplexer.registerListener(this);
		    }
		*/

		virtual OntologyValue optimalParameter( const OntologyAttribute & attribute ) {
			return OntologyValue( 42 );
		}


};

PLUGIN( OptimizerSkeleton )

