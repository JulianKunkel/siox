#ifndef STATISTICS_PLUGIN_DEREFERENCING_FACADE_H_
#define STATISTICS_PLUGIN_DEREFERENCING_FACADE_H_

#include <core/component/Component.hpp>

#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>

using namespace std;
using namespace core;
using namespace monitoring;

/*
 This class provides a facade for quering details for activities.
 */
namespace monitoring{

class StatisticsPluginDereferencing : public Component{
public:
    virtual OntologyAttribute * lookup_attribute_by_name(const string & domain, const string & name) = 0;

    virtual OntologyAttribute * lookup_attribute_by_ID(OntologyAttributeID aID) = 0;

    virtual const OntologyValue * lookup_meta_attribute(OntologyAttribute * attribute, OntologyAttribute * meta) = 0;

	virtual SystemInformationGlobalIDManager * get_system_information() = 0;
}; 
}

#define STATISTICS_DEREFERENCING_FACADE_INTERFACE "monitoring_statistics_plugin_dereferencing"


#endif
