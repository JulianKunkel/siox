#ifndef CONTAINER_SERIALIZER_HPP
#define CONTAINER_SERIALIZER_HPP

#include <sstream>
#include <string>

#include <core/container/container.hpp>


namespace core{

class ContainerSerializer{
public:
	std::string serialize(const Container * object);

	void serialize(const Container * object, std::ostream & os);

	Container * parse(std::istream & stream);

	Container * parse(std::string data);
};


}

#endif
