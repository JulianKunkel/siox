#include <core/component/component-macros.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>



using namespace std;
using namespace monitoring;

// You may or may not use this advanced options.
class AnomalyDetectionAbstractPluginOptions: public ActivityMultiplexerPluginOptions{
private:
	ComponentReference dereferenceFacade;
public:
	ActivityPluginDereferencing * dereferencingFacade(){
		return dereferenceFacade.instance<ActivityPluginDereferencing>();
	}

	SERIALIZE_CONTAINER(MEMBER(dereferenceFacade))
};

#undef SERIALIZE_CONTAINER
#define SERIALIZE_CONTAINER(VAR_) SERIALIZE(VAR_ PARENT_CLASS(AnomalyDetectionAbstractPluginOptions))
