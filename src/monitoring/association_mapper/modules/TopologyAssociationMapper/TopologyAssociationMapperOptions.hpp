#ifndef  TopologyAssociationMapperOptions_INC
#define  TopologyAssociationMapperOptions_INC

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace std;
using namespace core;

//@serializable
class TopologyAssociationMapperOptions: public core::ComponentOptions {
	public:
		ComponentReference topology;
};

#endif   /* ----- #ifndef TopologyAssociationMapperOptions_INC  ----- */
