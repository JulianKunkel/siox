#ifndef SIOX_COMPONENT_XML_SERIALIZABLE_S_H
#define SIOX_COMPONENT_XML_SERIALIZABLE_S_H


#include <core/component/ComponentReference.hpp>
#include <core/component/ComponentRegistrar.hpp>

#include <core/container/container-xml-serializer.hpp>

extern core::ComponentRegistrar * autoConfiguratorRegistrar;
extern int autoConfiguratorOffset;

namespace j_xml_serialization{

   void serialize(ComponentReference & t, stringstream & s, int intent = 0, const string & name = "ComponentReference"){
      storeTagBegin(s, name, intent);
      
      storeSimpleXMLTag(s, "componentID", t.componentID, intent + 1); 
      storeSimpleXMLTag(s, "global", t.global, intent + 1);

      storeTagEnd(s, name, intent);
   }



   void deserialize(ComponentReference & t, stringstream & s, const string & name = "ComponentReference"){
      checkXMLTagBegin(s, name, "");

      retrieveSimpleXMLTag(s, "componentID", t.componentID);
      retrieveSimpleXMLTag(s, "global", t.global);

      checkXMLTagEnd(s, name, "");

      // load component from ModuleRegistrar
      if( t.componentID != 0 ) {
         int myOffset = 0;
         if( ! t.global ) {
            myOffset = autoConfiguratorOffset;
         }
         t.componentPointer = ( ::core::Component* ) autoConfiguratorRegistrar->lookupComponent( t.componentID + myOffset );
      }
   }

}


#endif