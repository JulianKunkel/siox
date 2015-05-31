#ifndef  Replay_INC
#define  Replay_INC

#include <regex>
#include <unordered_map>
#include <memory>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>

#include "ReplayOptions.hpp"

class ReplayPlugin : public monitoring::ActivityMultiplexerPlugin {
	public:
		void initPlugin() override;
		ComponentOptions* AvailableOptions() override; 
		void finalize() override {};
		void replayActivity( std::shared_ptr<Activity> a );
	private:
		void strattribute( const Attribute & a, stringstream & s ) throw (NotFoundError);
		void notify(const std::shared_ptr<Activity>& a, int lost);
		SystemInformationGlobalIDManager* sys_info;
};

#endif   /* ----- #ifndef Replay_INC  ----- */
