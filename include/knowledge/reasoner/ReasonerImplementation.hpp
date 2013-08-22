#ifndef KNOWLEDGE_REASONER_IMPL_HPP
#define KNOWLEDGE_REASONER_IMPL_HPP

#include <knowledge/reasoner/Reasoner.hpp>

#define COMPONENT(x) \
	extern "C"{\
		void * get_instance_knowledge_reasoner() { return new x(); }\
	}

#endif
