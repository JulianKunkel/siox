#include <core/component/component-options.hpp>
#include <core/component/ComponentReference.hpp>

using namespace std;
using namespace core;

//@serializable
class TopologyOntologyOptions: public core::ComponentOptions {
	public:
		ComponentReference topology;
};

