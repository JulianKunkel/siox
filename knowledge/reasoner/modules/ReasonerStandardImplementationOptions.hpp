#include <core/component/component-macros.hpp>


using namespace std;

class ReasonerStandardImplementationOptions: public core::ComponentOptions{
public:
	uint32_t update_intervall_ms;
	SERIALIZE_CONTAINER(MEMBER(update_intervall_ms))
};


