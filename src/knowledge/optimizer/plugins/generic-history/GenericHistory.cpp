#include <iostream>
#include <list>
#include <unordered_map>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <knowledge/optimizer/OptimizerPluginInterface.hpp>
#include <knowledge/optimizer/Optimizer.hpp>

#include <workarounds.hpp>

#include "GenericHistoryOptions.hpp"


using namespace std;
using namespace monitoring;
using namespace core;
using namespace knowledge;



enum TokenType {
	OPEN = 0,
	ACCESS,
	CLOSE,
	HINT,
	UNKNOWN,
	TOKEN_TYPE_COUNT
};
ADD_ENUM_OPERATORS(TokenType)

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
		SystemInformationGlobalIDManager * sys;

		// bool tokensInitialized = false;

		unordered_map<UniqueComponentActivityID, TokenType>	types;

		int 	nTypes[TOKEN_TYPE_COUNT];


	public:

		GenericHistoryPlugin() : nTypes{} {
		}

		ComponentOptions * AvailableOptions() override {
		  return new GenericHistoryOptions();
		}

		void Notify( std::shared_ptr<Activity> activity ) override {
			cout << "GenericHistoryPlugin received " << activity << endl;
			/*
			OntologyAttribute & attribute;
			if( ! optimizer->isPluginRegistered( attribute ) ){
				optimizer->registerPlugin(attribute, & this);
			}
			*/

/*			TokenType type = UNKNOWN;
			try {
				type = types.at(activity->ucaid());
			}
			catch(...) {}
*/
			cout << "Generic History saw activity of type " << activity->ucaid() <<"\n";

/*			switch (type)
			{
				case OPEN: ;
				case ACCESS: ;
				case CLOSE: ;
				case HINT: ;
				default: ;
			}
*/
			// nTypes[type]++;
		}

		void initPlugin() override {
			fprintf(stderr, "GenericHistoryPlugin::initPlugin(), this = 0x%016jx\n", (intmax_t)this);
			GenericHistoryOptions & o = getOptions<GenericHistoryOptions>();
			optimizer = GET_INSTANCE(Optimizer, o.optimizer);

			sys = facade->get_system_information();
			assert(sys != nullptr);

			// filesize = dereferenceFacade->lookup_attribute_by_name("test", "filesize");
			// assert(filesize != nullptr); generally true but not for the first run.

			// Find Interface ID for MPI
			UniqueInterfaceID uiid;
/*			try{
				uiid = sys->lookup_interfaceID("MPI","2.1");
				/// @todo Look up MPI implementation; either from module, or by inspecting incoming activities
				/// until an MPI-specific one is found.
			}
			catch(NotFoundError)
			{
				cerr << "No UniqueInterfaceID for Interface MPI found - aborting!\n";
				abort();
			}

			// Fill token map with MPI UCAIDs and their type
			try{
				types[sys->lookup_activityID(uiid,"MPI_File_open")] = OPEN;

				types[sys->lookup_activityID(uiid,"MPI_File_read")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_read_all")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_read_at")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_read_at_all")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_read_ordered")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_read_shared")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_write")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_write_all")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_write_at")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_write_at_all")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_write_ordered")] = ACCESS;
				types[sys->lookup_activityID(uiid,"MPI_File_write_shared")] = ACCESS;

				types[sys->lookup_activityID(uiid,"MPI_File_close")] = CLOSE;

				types[sys->lookup_activityID(uiid,"MPI_File_set_info")] = HINT;
			}
			catch(NotFoundError)
			{
				cerr << "UniqueComponentActivityID for one or more MPI activities not found - aborting!\n";
				abort();
			}
*/		}

		virtual OntologyValue optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) override {

			cout << "Up to now, GenericHistoryPlugin saw\n";
			cout << "\t" << nTypes[OPEN] << " OPENs\n";
			cout << "\t" << nTypes[ACCESS] << " ACCESSs\n";
			cout << "\t" << nTypes[CLOSE] << " CLOSEs\n";
			cout << "\t" << nTypes[HINT] << " HINTs\n";

			return OntologyValue( 42 );
		}
};


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new GenericHistoryPlugin();
	}
}
