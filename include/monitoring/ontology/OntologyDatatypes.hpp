#ifndef SIOX_ONTOLOGY_DATATYPE_H
#define SIOX_ONTOLOGY_DATATYPE_H

#include <monitoring/datatypes/ids.hpp>

#include <string>
#include <vector>

using namespace std;

namespace monitoring{

namespace ontology{

typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, std::string, float, double> Value;

class Attribute {
public:
    OntologyAttributeID aID;

    string name;
    enum siox_ont_storage_type storage_type;

    Attribute(OntologyAttributeID aid, string & name, enum siox_ont_storage_type storage_type) : aID(aid), name(name), storage_type(storage_type) {}
};
}
}






#endif
