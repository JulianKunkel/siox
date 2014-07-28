#ifndef BOOST_SERIALIZABLE_Container
#define BOOST_SERIALIZABLE_Container

#include <core/container/container-macros.hpp>
#include <boost/serialization/serialization.hpp>

namespace boost {
	namespace serialization {
		template<class Archive>
		void serialize( Archive & ar, core::Container & g, const unsigned int version )
		{
		}
	}
}

#endif