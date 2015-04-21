#include <typeinfo>

#include <iostream>
#include <sstream>
#include <dlfcn.h>

#include <core/container/container-serializer.hpp>
#include <core/container/container.hpp>

using namespace std;


namespace core {

	string ContainerSerializer::serialize( Container * object )
	{
		stringstream s;
		serialize( object, s );
		return s.str();
	}

	void ContainerSerializer::parse(Container * out, string data )
	{
		stringstream s( data );
		parse( out, s );
	}

	void ContainerSerializer::serialize( Container * c, stringstream & s )
	{
		stringstream symbol;
		symbol << "_ZN19j_xml_serialization17WRAPPER_serializeI" << typeid(*c).name();
		symbol << "EEvPT_RSt18basic_stringstreamIcSt11char_traitsIcESaIcEE";

		void * fp= dlsym(NULL, symbol.str().c_str());
		if (fp == NULL){
			cerr << "ERROR could not find required symbol for container serialization " << symbol.str() << endl;
			exit(1);
		}
		// now invoke the wrapping code
		(*(void (*)(Container * c, stringstream & s))fp)(c, s);
	}

	void ContainerSerializer::parse(Container * c, stringstream & s )
	{
		stringstream symbol;
		symbol << "_ZN19j_xml_serialization19WRAPPER_deserializeI" << typeid(*c).name();
		symbol << "EEvPT_RSt18basic_stringstreamIcSt11char_traitsIcESaIcEE";

		void * fp= dlsym(NULL, symbol.str().c_str());
		if (fp == NULL){
			cerr << "ERROR could not find required symbol for container serialization " << symbol.str() << endl;
			exit(1);
		}
		//cout << "PARSE: " << symbol.str() << endl;
		// now invoke the wrapping code
		(*(void (*)(Container * c, stringstream & s))fp)(c, s);
	}
}
