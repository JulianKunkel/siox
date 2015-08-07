#include <typeinfo>

#include <iostream>
#include <sstream>
#include <dlfcn.h>

#include <core/container/container-serializer.hpp>
#include <core/container/container.hpp>

using namespace std;


namespace core {

   typedef void(*wrapperCode)(Container * c, stringstream & s); 

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

   static wrapperCode findSymbol(bool serialize, const string & name){
      stringstream symbol;
      string substr;

      if (serialize){
         substr = "17WRAPPER_serializeI";
      }else{
         substr = "19WRAPPER_deserializeI";
      }

      symbol << "_ZN19j_xml_serialization" << substr << name;
      symbol << "EEvPT_RSt18basic_stringstreamIcSt11char_traitsIcESaIcEE";
      void * fp= dlsym(NULL, symbol.str().c_str());
      if (fp == NULL){
         // check second alternative, since GCC 5.1
         symbol = stringstream();
         symbol << "_ZN19j_xml_serialization" << substr << name;
         symbol << "EEvPT_RNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE";
         fp = dlsym(NULL, symbol.str().c_str());

         if (fp == NULL){
            cerr << "ERROR could not find required symbol for container serialization " << symbol.str() << endl;
            exit(1);
         }
      }
      return (wrapperCode) fp;
   }

	void ContainerSerializer::serialize( Container * c, stringstream & s )
	{
      wrapperCode fp = findSymbol(true, typeid(*c).name());
      // now invoke the wrapping code
      (*fp)(c, s);
	}

	void ContainerSerializer::parse(Container * c, stringstream & s )
	{
      wrapperCode fp = findSymbol(false, typeid(*c).name());
      // now invoke the wrapping code
      (*fp)(c, s);      
	}
}
