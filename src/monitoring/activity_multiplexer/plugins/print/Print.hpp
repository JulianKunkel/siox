#ifndef  Print_INC
#define  Print_INC

#include <regex>
#include <unordered_map>
#include <memory>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <tools/TraceReader/CommandLineOptions.hpp>

#include "PrintOptions.hpp"

class PrintPlugin : public monitoring::ActivityMultiplexerPlugin, public CommandLineOptions {
	public:
		void moduleOptions(boost::program_options::options_description& od) override;
		void setOptions(const boost::program_options::variables_map& vm) override;
		void initPlugin() override;
		ComponentOptions* AvailableOptions() override; 
		void finalize() override {};
		void printActivity( std::shared_ptr<Activity> a );
	private:
		void strattribute( const Attribute & a, stringstream & s ) throw (NotFoundError);
		void notify(const std::shared_ptr<Activity>& a, int lost);
		SystemInformationGlobalIDManager* sys_info;
//		AssociationMapper* association_mapper;
//		Ontology* ontology;
		
};

#endif   /* ----- #ifndef Print_INC  ----- */
