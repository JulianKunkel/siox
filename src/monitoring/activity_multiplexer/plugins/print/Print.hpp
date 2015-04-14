#ifndef  Print_INC
#define  Print_INC

#include <regex>
#include <unordered_map>
#include <memory>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>

#include "PrintOptions.hpp"

class PrintPlugin : public monitoring::ActivityMultiplexerPlugin {
	public:
		void initPlugin() override;
		ComponentOptions* AvailableOptions() override; 
		void finalize() override {};
		void printActivity( std::shared_ptr<Activity> a );
	private:
		void strattribute( const Attribute & a, stringstream & s ) throw (NotFoundError);
		void notify(const std::shared_ptr<Activity>& a, int lost);
		SystemInformationGlobalIDManager* sys_info;
};

#endif   /* ----- #ifndef Print_INC  ----- */
