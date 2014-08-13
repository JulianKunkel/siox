/**
 * Topologoy that holds its data in a SQL-database
 *
 * @author Roman Michel
 * @date 2013
 */
#include <sstream>
#include <pqxx/pqxx>

#include <core/persist/SetupPersistentStructures.hpp>

#include <util/ExceptionHandling.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>


#include <boost/algorithm/string.hpp>

#include "DatabaseTopologyOptions.hpp"

using namespace std;
using namespace core;
using namespace monitoring;
using namespace pqxx;
using namespace boost;

class DatabaseTopology : public Topology, public SetupPersistentStructures {
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

        int preparePersistentStructuresIfNecessary() override;

    int cleanPersistentStructures() override;

    private:
        connection* conn;
};

int DatabaseTopology::preparePersistentStructuresIfNecessary(){
    bool ret = true;
    // TODO
    return ret;
}

int DatabaseTopology::cleanPersistentStructures(){
    bool ret = true;
    // TODO
    return ret;
}

void DatabaseTopology::start(){
    Topology::start();

    if (conn) return;

    DatabaseTopologyOptions & o = getOptions<DatabaseTopologyOptions>();
    stringstream endpoint;
    endpoint << "hostaddr=" << o.hostaddress << " port=" << o.port << " user=" << o.username << " password=" << o.password << " dbname=" << o.dbname;

    conn = new connection(endpoint.str());
}

void DatabaseTopology::stop(){
    if (conn){
      conn->disconnect();
      conn = nullptr;  
    } 
    Topology::stop();
}


void DatabaseTopology::init() {
    start();
}

ComponentOptions* DatabaseTopology::AvailableOptions() {
    return new DatabaseTopologyOptions();
}

TopologyType DatabaseTopology::registerType( const string& name ) throw() {
    TopologyType tmpType;

    // Check if Type is in database
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "registerType");
    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM topology.type WHERE name='" + selectAction.esc(name) + "'"));

    // Check if there is only one result, if so -> return the object
    if (resultSelect.size() == 1) {
        TopologyTypeId tmpTypeID;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpTypeID)) {
            assert(false);
        }

        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, tmpTypeID));
        tmpType.setObject(newType);

        selectAction.commit();

        return tmpType;
    }
    else if (resultSelect.size() != 0) {
        // Something very strange happened
        assert(false);
    }

    // It's not in the database, perform an insert
    resultSelect = selectAction.exec("INSERT INTO topology.type (name) VALUES ('"+selectAction.esc(name)+"') returning id");

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyTypeId tmpTypeID;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpTypeID)) {
            assert(false);
        }

        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, tmpTypeID));
        tmpType.setObject(newType);

        selectAction.commit();

        return tmpType;
    }
    else {
        // Something very strange happened
        assert(false);
    }

   return tmpType;
}

TopologyType DatabaseTopology::lookupTypeByName( const string& name ) throw() {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "lookupTypeByName");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM topology.Type WHERE name='" + selectAction.esc(name) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyTypeId tmpId;
        TopologyType tmpType;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            assert(false);
        }

        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, tmpId));
        tmpType.setObject(newType);

        return tmpType;
    }
    else {
        // Not found
        return TopologyType();
    }
}

TopologyType DatabaseTopology::lookupTypeById( TopologyTypeId anId ) throw() {
    string tmpName;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "lookupTypeById");

    // Perform a select
    result resultSelect = selectAction.exec("SELECT name FROM topology.Type WHERE id='"+to_string(anId)+"'");
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyType tmpType;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["name"].to(tmpName)) {
            assert(false);
        }
        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(tmpName));
        newType->id = anId;
        tmpType.setObject(newType);
        return tmpType;

    }
    else {
        // Not found
        return TopologyType();
    }
}

TopologyObject DatabaseTopology::registerObject( TopologyObjectId parentId, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw() {

    work selectAction(*conn, "registerObject");
    // Perform a select
    result resultSelect = selectAction.exec(("SELECT childObjectId FROM topology.relation WHERE childName='" + selectAction.esc(childName) + "' AND parentObjectId = '"+to_string(parentId)+"' AND relationTypeId = '"+to_string(relationType)+"'"));

    // Check if there is only one result, if so -> return the object
    if (resultSelect.size() == 1) {
        TopologyObjectId tmpId;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["childObjectId"].to(tmpId)) {
            assert(false);
        }

        selectAction.commit();

        TopologyObject tmpObject;
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(objectType, tmpId) );
        tmpObject.setObject(newObject);

        return tmpObject;
    }
    else if (resultSelect.size() != 0) {
        // Something very strange happened
        assert(false);
    }

    // Perform the insert
    resultSelect = selectAction.exec("INSERT INTO topology.Object (typeId) VALUES ('"+to_string(objectType)+"') returning id" );

    // Check if there is only one result
    if (resultSelect.size() == 1) {
         TopologyObjectId tmpId;

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            assert(false);
        }

        selectAction.commit();

        TopologyObject tmpObject;
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(objectType, tmpId) );
        tmpObject.setObject(newObject);

        registerRelation(parentId, relationType, childName, tmpId );
        return tmpObject;
    }
    else {
        assert(false);
    }

   return tmpObject;
}

TopologyObject DatabaseTopology::lookupObjectById( TopologyObjectId anId ) throw() {
    TopologyObject tmpObject;
    TopologyObjectId tmpId;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "lookupObjectById");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT typeId FROM topology.Object WHERE id='" + to_string(anId) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["TypeId"].to(tmpId)) {
            assert(false);
        }
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(tmpId, anId) );
        tmpObject.setObject(newObject);
    }
    else {
        // Not found, let empty object return
    }

    return tmpObject;
}

TopologyRelation DatabaseTopology::registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw() {
    TopologyRelation tmpRelation;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "registerRelation");

    stringstream buff;
    buff << "SELECT childObjectID FROM topology.Relation WHERE childName='" << insertAction.esc(childName) << "' AND parentObjectId = '" << parent << "' AND relationTypeId='" << relationType << "'";
    // Perform a select
    result resultSelect = insertAction.exec(buff.str());
    // Check if there is only one result
    if (resultSelect.size() > 0) {
        insertAction.commit();
        Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );
        tmpRelation.setObject(newRelation);
        return tmpRelation;
    }
    else {
        // Perform another insert
        insertAction.exec("INSERT INTO topology.Relation (parentObjectId, childName, childObjectId, relationTypeId) VALUES ('"+to_string(parent)+"','"+insertAction.esc(childName)+"','"+to_string(child)+"','"+to_string(relationType)+"')");
        insertAction.commit();

        Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );
        tmpRelation.setObject(newRelation);
        return tmpRelation;
    }
}

TopologyRelation DatabaseTopology::lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() {
    TopologyObjectId tmpChild;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "lookupRelation");

    stringstream buff;
    buff << "SELECT childObjectID FROM topology.Relation WHERE childName='" << selectAction.esc(childName) << "' AND parentObjectId = '" << parent << "' AND relationTypeId='" << relationType << "'";
    // Perform a select
    result resultSelect = selectAction.exec(buff.str());
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyRelation tmpRelation;

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["childObjectID"].to(tmpChild)) {
            assert(false);
        }
        Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, tmpChild, relationType ) );
        tmpRelation.setObject(newRelation);
        return tmpRelation;
    }
    else {
        // Not found
        return TopologyRelation();
    }

}

TopologyRelationList DatabaseTopology::enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() {

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    /*work selectAction(*conn, "Select Transaction");

    result resultSelect = selectAction.exec(("SELECT childObjectID, childName FROM Relation WHERE relationTypeId='" + to_string(relationType) + "' AND parentObjectId = '"+ to_string(parent) + "'"));
    selectAction.commit();

    TopologyRelationList returnVector;
    string tmpChildName;
    TopologyObjectId tmpChild;
    TopologyRelation tmpRelation;

    // Check if there is only one result
    if (resultSelect.size() > 0) {
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            if (!resultSelect[i]["childObjectID"].to(tmpChild) &&
                !resultSelect[i]["childName"].to(tmpChildName)) {
                assert(false);
            }

            Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( tmpChildName, parent, tmpChild, relationType ) );
            tmpRelation.setObject(newRelation);
            returnVector.emplace_back(tmpRelation);
        }
    }
    else {
        // Not found
    }
    return returnVector;*/
    assert(false);

    TopologyRelationList returnVector;
    return returnVector;

}

TopologyRelationList DatabaseTopology::enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    /*work selectAction(*conn, "Select Transaction");

    result resultSelect = selectAction.exec(("SELECT parentObjectID, childName FROM Relation WHERE childObjectId='" + to_string(child) + "' AND relationTypeId = '"+ to_string(relationType) + "'"));
    selectAction.commit();

    TopologyRelationList returnVector;
    string tmpChildName;
    TopologyObjectId tmpParent;
    TopologyRelation tmpRelation;

    // Check if there is only one result
    if (resultSelect.size() > 0) {
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            if (!resultSelect[i]["parentObjectID"].to(tmpParent)) {
                assert(false);
            }
            if (!resultSelect[i]["childName"].to(tmpChildName)) {
                assert(false);
            }

            Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( tmpChildName, tmpParent, child, relationType ) );
            tmpRelation.setObject(newRelation);
            returnVector.emplace_back(tmpRelation);
        }
    }
    else {
        // Not found
    }*/

    assert(false);

    TopologyRelationList returnVector;
    return returnVector;
}

TopologyAttribute DatabaseTopology::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw() {

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "registerAttribute");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id, dataType FROM topology.Attribute WHERE name='" + selectAction.esc(name) + "' AND domainTypeId = '"+to_string(domain)+"'"));

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyAttribute tmpAttribute;
        TopologyTypeId tmpId;
        intmax_t tmpInt;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            assert(false);
        }
        if (!resultSelect[0]["dataType"].to(tmpInt)) {
            assert(false);
        }

        selectAction.commit();

        VariableDatatype::Type tmpType = (VariableDatatype::Type) tmpInt;
        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, tmpType ) );
        newAttribute->id = tmpId;
        tmpAttribute.setObject(newAttribute);
        return tmpAttribute;
    }
    else if(resultSelect.size() != 0) {
        // Something went horribly wrong
        assert(false);
    }

    // Dataset not found -> Perform an insert
    resultSelect = selectAction.exec("INSERT INTO topology.Attribute (name, domainTypeId, dataType) VALUES ('"+selectAction.esc(name)+"','"+to_string(domain)+"','"+to_string((intmax_t) datatype)+"') returning id");

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyAttribute tmpAttribute;
        TopologyTypeId tmpId;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            assert(false);
        }

        selectAction.commit();

        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, datatype ) );
        newAttribute->id = tmpId;
        tmpAttribute.setObject(newAttribute);
        return tmpAttribute;
    }
    else {
        // Something went horribly wrong.
        assert(false);
    }
    
    return tmpAttribute;
}

TopologyAttribute DatabaseTopology::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "lookupAttributeByName");
    TopologyTypeId tmpId;
    intmax_t tmpInt;

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id, dataType FROM topology.Attribute WHERE name='" + selectAction.esc(name) + "' AND domainTypeId = '"+to_string(domain)+"'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyAttribute tmpAttribute;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            assert(false);
        }
        if (!resultSelect[0]["dataType"].to(tmpInt)) {
            assert(false);
        }
        VariableDatatype::Type tmpType = (VariableDatatype::Type) tmpInt;
        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, tmpType ) );
        newAttribute->id = tmpId;
        tmpAttribute.setObject(newAttribute);
        return tmpAttribute;
    }
    else {
        // Not found
        return TopologyAttribute();
    }
}

TopologyAttribute DatabaseTopology::lookupAttributeById( TopologyAttributeId attributeId ) throw() {
    TopologyAttribute tmpAttribute;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "lookupAttributeById");
    string tmpName;
    int tmpId;
    intmax_t tmpInt;

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT name, domaintypeid, dataType FROM topology.Attribute WHERE id='" + to_string(attributeId) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["name"].to(tmpName)) {
            assert(false);
        }
        if (!resultSelect[0]["domaintypeid"].to(tmpId)) {
            assert(false);
        }
        if (!resultSelect[0]["dataType"].to(tmpInt)) {
            assert(false);
        }
        VariableDatatype::Type tmpType = (VariableDatatype::Type) tmpInt;
        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( tmpName, tmpId, tmpType ) );
        newAttribute->id = attributeId;
        tmpAttribute.setObject(newAttribute);
    }
    else {
        // Not found
    }

    return tmpAttribute;
}

bool DatabaseTopology::setAttribute( TopologyObjectId objectId, TopologyAttributeId attributeId, const TopologyVariable& value ) throw() {
    work insertAction(*conn, "setAttribute");
    // Perform an insert OR update the existing value
    stringstream buff;

    buff << "UPDATE topology.Value SET value='" << value.toStr() << "' WHERE objectid=" << objectId << " AND attributeid=" << attributeId << " returning objectid"; // returning objectid is a trick to get a row back

    result resultSelect = insertAction.exec(buff.str() );
    if (resultSelect.size() == 1) {
        insertAction.commit();

        return true;
    }

    stringstream insert;
    insert << "INSERT INTO topology.Value (objectId, attributeId, type, value) VALUES ('" << objectId << "','" << attributeId << "','" << ((int) value.type()) << "','" << value.toStr() << "')";
    insertAction.exec(insert.str() );
    insertAction.commit();

    return true;
}

TopologyValue DatabaseTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() {
    work selectAction(*conn, "getAttribute");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT value, type FROM topology.Value WHERE objectId='" + to_string(object) + "' AND attributeId = '"+to_string(attribute)+"'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        string valueString;
        int type;

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["value"].to(valueString)) {
            assert(false);
        }
        if (!resultSelect[0]["type"].to(type)) {
            assert(false);
        }
        TopologyValue value;
        VariableDatatype convertedValue((VariableDatatype::Type) type, valueString);

        Release<TopologyValueImplementation> newValue( new TopologyValueImplementation( convertedValue, object, attribute ) );
        value.setObject(newValue);
        return value;
    }
    else {
        // Not found
        return TopologyValue();
    }
}

TopologyValueList DatabaseTopology::enumerateAttributes( TopologyObjectId object ) throw() {
    /*work selectAction(*conn, "Select Transaction");
    string valueString;

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT value FROM Value WHERE objectId='" + to_string(object) + "'"));
    selectAction.commit();

    string tmpValueString;
    TopologyVariable *tmpValue;
    Topology::TopologyValueList returnVector;

    // Check if there is only one result
    if (resultSelect.size() > 0) {
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            if (!resultSelect[i]["value"].to(tmpValueString)) {
                assert(false);
            }

            tmpValue = deserializeTopologyVariable(tmpValueString);
            returnVector.emplace_back(*tmpValue);
        }
    }
    else {
        // Not found
    }*/
    assert(false);

    Topology::TopologyValueList returnVector;
    return returnVector;
}

extern "C" {
    void* TOPOLOGY_INSTANCIATOR_NAME()
    {
        return new DatabaseTopology();
    }
}
