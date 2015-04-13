#ifndef  TopologyOntologyOptions_INC
#define  TopologyOntologyOptions_INC

#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace std;
using namespace core;

//@serializable
class TopologyOntologyOptions: public core::ComponentOptions {
	public:
		ComponentReference topology;
};

#endif   /* ----- #ifndef TopologyOntologyOptions_INC  ----- */
