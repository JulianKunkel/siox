#include <core/component/component-options.hpp>

using namespace std;

//@serializable
class CommASIOOptions: public core::ComponentOptions {
	public:
		int thread_count;
};

