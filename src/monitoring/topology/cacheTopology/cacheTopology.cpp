/**
 * Topologoy that caches data in RAM but also forwards it to another topology implementation
 *
 * @author Roman Michel
 * @date 2014
 */
#include <sstream>

#include <util/ExceptionHandling.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>


#include <boost/algorithm/string.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "cacheTopologyOptions.hpp"
#include "cacheHelper.cpp"

using namespace std;
using namespace core;
using namespace monitoring;
using namespace boost;

class CacheTopology : public Topology {
    public:
        virtual void init();
        virtual ComponentOptions * AvailableOptions();

        virtual TopologyType registerType( const string& name ) throw();
        virtual TopologyType lookupTypeByName( const string& name ) throw();
        virtual TopologyType lookupTypeById( TopologyTypeId anId ) throw();
        virtual TopologyObject registerObject( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw();
        virtual TopologyObject lookupObjectById( TopologyObjectId anId ) throw();

        virtual TopologyRelation registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw();
        virtual TopologyRelation lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw();


        virtual TopologyRelationList enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw();
        virtual TopologyRelationList enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw();


        virtual TopologyAttribute registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw();
        virtual TopologyAttribute lookupAttributeByName( TopologyTypeId domain, const string& name ) throw();
        virtual TopologyAttribute lookupAttributeById( TopologyAttributeId attributeId ) throw();
        virtual bool setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyVariable& value ) throw();
        virtual TopologyValue getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw();
        virtual TopologyValueList enumerateAttributes( TopologyObjectId object ) throw();

        // this module is mandatory! => it will only be stopped for fork()
        bool isMandatoryModule() override{ 
                return true;
        }

        void start() override;
        void stop() override;

    private:
        CacheHelper* ramCache;
        Topology* topologyBackend;
};

void CacheTopology::start(){
    Topology::start();
    
    ramCache = new CacheHelper();
}

void CacheTopology::stop(){
    // TODO: Destroyed das so?
    ramCache = nullptr;
    Topology::stop();
}


void CacheTopology::init() {
    CacheTopologyOptions & o = getOptions<CacheTopologyOptions>();

    topologyBackend = GET_INSTANCE(Topology, o.topologyBackend);    
}

ComponentOptions* CacheTopology::AvailableOptions() {
    return new CacheTopologyOptions();
}

TopologyType CacheTopology::registerType( const string& name ) throw() {
    TopologyType tmpType;

    // Look if it is already in the cache
    tmpType = ramCache->lookupTypeByName(name);
    if (!tmpType) {
        // It isn't in the cache -> invoke other topology
        tmpType = topologyBackend->registerType(name);
        if (!tmpType) {
            // If the other topology successfully returned, insert the object into the cache
            ramCache->registerType(name, tmpType.id());
        }
    }

    return tmpType;
}

TopologyType CacheTopology::lookupTypeByName( const string& name ) throw() {
    TopologyType tmpType;

    // Look if it is already in the cache
    tmpType = ramCache->lookupTypeByName(name);
    if (!tmpType) {
        // It isn't in the cache -> invoke other topology
        tmpType = topologyBackend->lookupTypeByName(name);
        if (!tmpType) {
            // If the other topology successfully returned, insert the object into the cache
            ramCache->registerType(name, tmpType.id());
        }
    }

    return tmpType;
}

TopologyType CacheTopology::lookupTypeById( TopologyTypeId anId ) throw() {
    TopologyType tmpType;

    // Look if it is already in the cache
    tmpType = ramCache->lookupTypeById(anId);
    if (!tmpType) {
        // It isn't in the cache -> invoke other topology
        tmpType = topologyBackend->lookupTypeById(anId);
        if (!tmpType) {
            // If the other topology successfully returned, insert the object into the cache
            ramCache->registerType(tmpType.name(), anId);
        }
    }

    return tmpType;
}

TopologyObject CacheTopology::registerObject( TopologyObjectId parentId, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw() {
    TopologyObject tmpObject;
    TopologyRelation tmpRelation;

    // It is impossible to lookup an object without its ID, so we look if there a relation available
    tmpRelation = ramCache->lookupRelation(parentId, relationType, childName);
    if (!tmpRelation) {
        // No relation available, invoke other topology
        tmpObject = topologyBackend->registerObject(parentId, relationType, childName, objectType);
        if (!tmpObject) {
            // Insert into cache
            ramCache->registerObject(tmpObject.type(), tmpObject.id());
            ramCache->registerRelation(parentId, relationType, childName, tmpObject.id());
        }
    }
    else {
        // Relation available, return object
        tmpObject = ramCache->lookupObjectById(tmpRelation.child());
        if (!tmpObject) {
            // Our cache doesn't have this object, get it out of the other topology
            tmpObject = topologyBackend->registerObject(parentId, relationType, childName, objectType);
        }
    }

    return tmpObject;
}

TopologyObject CacheTopology::lookupObjectById( TopologyObjectId anId ) throw() {
    TopologyObject tmpObject;

    // Look if it is already in the cache
    tmpObject = ramCache->lookupObjectById(anId);
    if (!tmpObject) {
        // It's not in the cache -> invoke other topology
        tmpObject = topologyBackend->lookupObjectById(anId);
        if (tmpObject) {
            // TODO: Macht das schreiben ohne die Relation Sinn?
            // Insert into cache
            ramCache->registerObject(tmpObject.type(), tmpObject.id());
        }
    }

    return tmpObject;
}

TopologyRelation CacheTopology::registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw() {
    TopologyRelation tmpRelation;

    // Look if it is already in the cache
    tmpRelation = ramCache->lookupRelation(parent, relationType, childName);
    if (!tmpRelation) {
        // It's not in the cache -> invoke other topology
        tmpRelation = topologyBackend->registerRelation(parent, relationType, childName, child);
        if (!tmpRelation) {
            // Insert into cache
            ramCache->registerRelation(parent, relationType, childName, child);
        }
    }

    return tmpRelation;
}

TopologyRelation CacheTopology::lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() {
    TopologyRelation tmpRelation;

    // Look if it is already in the cache
    tmpRelation = ramCache->lookupRelation(parent, relationType, childName);
    if (!tmpRelation) {
        // It's not in the cache -> invoke other topology
        tmpRelation = topologyBackend->lookupRelation(parent, relationType, childName);

        if(!tmpRelation) {
            // Insert into cache
            ramCache->registerRelation(parent, relationType, childName, tmpRelation.child());
        }
    }
    
    return tmpRelation;
}

TopologyRelationList CacheTopology::enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() {
    // We can't cache this
    return topologyBackend->enumerateChildren(parent, relationType);
}

TopologyRelationList CacheTopology::enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() {
    // We can't cache this
    return topologyBackend->enumerateParents(child, relationType);
}

TopologyAttribute CacheTopology::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw() {
    TopologyAttribute tmpAttribute;

    // Look if it is already in the cache
    tmpAttribute = ramCache->lookupAttributeByName(domain, name);
    if (!tmpAttribute) {
        // It's not in the cache -> invoke other topology
        tmpAttribute = topologyBackend->registerAttribute(domain, name, datatype);

        if(!tmpAttribute) {
            // Insert into cache
            ramCache->registerAttribute(domain, name, datatype, tmpAttribute.id());
        }
    }
    return tmpAttribute;
}

TopologyAttribute CacheTopology::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() {
    TopologyAttribute tmpAttribute;

    // Look if it is already in the cache
    tmpAttribute = ramCache->lookupAttributeByName(domain, name);
    if (!tmpAttribute) {
        // It's not in the cache -> invoke other topology
        tmpAttribute = topologyBackend->lookupAttributeByName(domain, name);

        if(!tmpAttribute) {
            // Insert into cache
            ramCache->registerAttribute(domain, name, tmpAttribute.dataType(), tmpAttribute.id());
        }
    }
    return tmpAttribute;
}

TopologyAttribute CacheTopology::lookupAttributeById( TopologyAttributeId attributeId ) throw() {
    TopologyAttribute tmpAttribute;

    // Look if it is already in the cache
    tmpAttribute = ramCache->lookupAttributeById(attributeId);
    if (!tmpAttribute) {
        // It's not in the cache -> invoke other topology
        tmpAttribute = topologyBackend->lookupAttributeById(attributeId);

        if (!tmpAttribute) {
            // Insert into cache
            ramCache->registerAttribute(tmpAttribute.domainId(), tmpAttribute.name(), tmpAttribute.dataType(), attributeId);
        }
    }
    return tmpAttribute;
}

bool CacheTopology::setAttribute( TopologyObjectId objectId, TopologyAttributeId attributeId, const TopologyVariable& value ) throw() {
    
    VariableDatatype convertedValue((VariableDatatype::Type) value.type(), value.toStr());
    
    // Look if it is already in the cache
    TopologyValue tmpValue = ramCache->registerValue(objectId, attributeId, convertedValue);

    if (!tmpValue) {
        return false;
    }
    
    if (topologyBackend->setAttribute(objectId, attributeId, value)) {
        return true;
    }

    return false;
}

TopologyValue CacheTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() {
    TopologyValue value;

    // Look if it is already in the cache
    value = ramCache->getValue(object, attribute);
    if (!value) {
        // Not there -> invoke other topology
        value = topologyBackend->getAttribute(object, attribute);

        if (!value) {
            // Insert into cache
            VariableDatatype convertedValue((VariableDatatype::Type) value.value().type(), value.value().toStr());
            ramCache->registerValue(object, attribute, convertedValue);
        }
    }
    
    return value;
}

TopologyValueList CacheTopology::enumerateAttributes( TopologyObjectId object ) throw() {
    // We can't cache this
    return topologyBackend->enumerateAttributes(object);
}

extern "C" {
    void* TOPOLOGY_INSTANCIATOR_NAME()
    {
        return new CacheTopology();
    }
}
