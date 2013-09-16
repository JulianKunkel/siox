#include <core/component/component-options.hpp>

using namespace std;

//@serializable
class FileBasedSystemInformationOptions: public core::ComponentOptions {
	public:
		string filename;
};

