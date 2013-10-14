#ifndef CONTAINER_SERIALIZABLE_H
#define CONTAINER_SERIALIZABLE_H

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