#ifndef CONTAINER_SERIALIZER_HPP
#define CONTAINER_SERIALIZER_HPP

#include <sstream>
#include <string>

#include <core/container/container.hpp>


namespace core {

	/**
	 * Serialize a container into/from XML
	 */
	class ContainerSerializer {
		public:
			std::string serialize( Container * object );

			void serialize( Container * object, std::stringstream & s );

			void parse( Container * out, std::stringstream & s );

			void parse( Container * out, std::string data );

	};

}

#endif
