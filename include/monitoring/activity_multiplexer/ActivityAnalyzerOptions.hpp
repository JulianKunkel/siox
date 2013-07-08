#include <core/component/component-macros.hpp>


#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>



using namespace std;
using namespace monitoring;

class AnomalyDetectionAbstractPluginOptions: public ActivityMultiplexerPluginOptions{
private:
	ComponentReference dereferenceFacade;
public:
	ActivityPluginDereferencing * dereferencingFacade(){
		return dereferenceFacade.instance<ActivityPluginDereferencing>();
	}

	SERIALIZE_CONTAINER(MEMBER(filename))
};


