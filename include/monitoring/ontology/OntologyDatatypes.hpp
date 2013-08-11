#ifndef SIOX_ONTOLOGY_DATATYPE_H
#define SIOX_ONTOLOGY_DATATYPE_H

#include <string>
#include <vector>
#include <string.h>
#include <assert.h>

#include <monitoring/datatypes/ids.hpp>
#include <core/datatypes/VariableDatatype.hpp>

using namespace std;

namespace monitoring{

typedef VariableDatatype OntologyValue;

class OntologyAttribute {
public:
    OntologyAttributeID aID;

	/**
	 * Reserved domains:
	 * - unit
	 *	for units of attribute values
	 *	(attach the unit as meta attribute of type SIOX_STORAGE_STRING to base attribute)
	 */
    string domain;
    string name;
    enum VariableDatatype::Type storage_type;
    //OntologyAttribute(OntologyAttributeID aid, string & name, enum siox_ont_storage_type storage_type) : aID(aid), name(name), storage_type(storage_type) {}

    inline bool operator==(OntologyAttribute const & a) const{
   		return ( a.aID == this->aID);
   	}

    inline bool operator!=(OntologyAttribute const & a) const{
   		return ! ( a.aID == this->aID);
   	}
};
}

#endif
