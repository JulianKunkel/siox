/**
 * Topologoy that holds its data in a SQL-database
 *
 * @author Roman Michel
 * @date 2013
 */

#include <util/ExceptionHandling.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>

#include <pqxx/pqxx>
#include <boost/algorithm/string.hpp>

#include "DatabaseTopologyOptions.hpp"

#include <sstream>

using namespace std;
using namespace core;
using namespace monitoring;
using namespace pqxx;
using namespace boost;

class DatabaseTopology : public Topology {
    public:
        ~DatabaseTopology();

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

    private:
        connection* conn;
};

DatabaseTopology::~DatabaseTopology() {
    conn->disconnect();
}

void DatabaseTopology::init() {
    DatabaseTopologyOptions & o = getOptions<DatabaseTopologyOptions>();
    stringstream endpoint;
    endpoint << "hostaddr=" << o.hostaddress << " port=" << o.port << " user=" << o.username << " password=" << o.password << " dbname=" << o.dbname;

    conn = new connection(endpoint.str());
}

ComponentOptions* DatabaseTopology::AvailableOptions() {
    return new DatabaseTopologyOptions();
}

TopologyType DatabaseTopology::registerType( const string& name ) throw() {
    TopologyType tmpType;

    tmpType = lookupTypeByName(name);
    if( tmpType ){
        return tmpType;
    }

    // TODO: Check if already in the database?
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "Insert Transaction");

    // Perform the insert
    insertAction.exec("INSERT INTO type (name) VALUES ('"+insertAction.esc(name)+"')");
    insertAction.commit();


    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");
    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM type WHERE name='" + selectAction.esc(name) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyTypeId tmpTypeID;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpTypeID)) {
            // TODO: ERROR
            assert(false);
        }

        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name, tmpTypeID));
        tmpType.setObject(newType);
        return tmpType;        
    }
    else {
        // TODO: ERROR
        assert(false);
    }
}

TopologyType DatabaseTopology::lookupTypeByName( const string& name ) throw() {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM Type WHERE name='" + selectAction.esc(name) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyTypeId tmpId;
        TopologyType tmpType;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            // TODO: ERROR
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
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec("SELECT name FROM Type WHERE id='"+to_string(anId)+"'");
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyType tmpType;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["name"].to(tmpName)) {
            // TODO: ERROR
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

    TopologyRelation tmpRelation = lookupRelation(parentId, relationType, childName);
    if(tmpRelation){
        return lookupObjectById(tmpRelation.child());
    }

    Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation( objectType ) );

    // TODO: Check if already in the database?
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "Insert Transaction");

    // Perform the insert
    result resultSelect = insertAction.exec("INSERT INTO Object (typeId) VALUES ('"+to_string(objectType)+"') returning id" );
    insertAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
         TopologyObjectId tmpId;

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            // TODO: ERROR
            assert(false);
        }
        TopologyObject tmpObject;
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(objectType, tmpId) );
        tmpObject.setObject(newObject);  

        registerRelation(parentId, relationType, childName, tmpId );
        return tmpObject;
    }
    else {
        // TODO: ERROR
        assert(false);
    }
}

TopologyObject DatabaseTopology::lookupObjectById( TopologyObjectId anId ) throw() {
    TopologyObject tmpObject;
    TopologyObjectId tmpId;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT typeId FROM Object WHERE id='" + to_string(anId) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["TypeId"].to(tmpId)) {
            // TODO: ERROR
            assert(false);
        }
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(tmpId, tmpId) );
        tmpObject.setObject(newObject);
    }
    else {
        // TODO: ERROR
    }

    return tmpObject;
}

TopologyRelation DatabaseTopology::registerRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName, TopologyObjectId child ) throw() {
    TopologyRelation tmpRelation;

    // TODO: Check if already in the database?
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "Insert Transaction");

    stringstream buff;    
    buff << "SELECT childObjectID FROM Relation WHERE childName='" << insertAction.esc(childName) << "' AND parentObjectId = '" << parent << "' AND relationTypeId='" << relationType << "'";
    // Perform a select
    result resultSelect = insertAction.exec(buff.str());
    // Check if there is only one result
    if (resultSelect.size() == 1) {        
        insertAction.commit();
        Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );
        tmpRelation.setObject(newRelation);
        return tmpRelation;
    }

    // Perform another insert
    insertAction.exec("INSERT INTO Relation (parentObjectId, childName, childObjectId, relationTypeId) VALUES ('"+to_string(parent)+"','"+insertAction.esc(childName)+"','"+to_string(child)+"','"+to_string(relationType)+"')");
    insertAction.commit();

    Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );
    tmpRelation.setObject(newRelation);
    return tmpRelation;
}

TopologyRelation DatabaseTopology::lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() {
    TopologyObjectId tmpChild;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    stringstream buff;
    buff << "SELECT childObjectID FROM Relation WHERE childName='" << selectAction.esc(childName) << "' AND parentObjectId = '" << parent << "' AND relationTypeId='" << relationType << "'";
    // Perform a select
    result resultSelect = selectAction.exec(buff.str());
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyRelation tmpRelation;

        // Check if results are sane (and convert them)
        // TODO: Conversion okay?
        if (!resultSelect[0]["childObjectID"].to(tmpChild)) {
            // TODO: ERROR
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
    work selectAction(*conn, "Select Transaction");

    result resultSelect = selectAction.exec(("SELECT childObjectID, childName FROM Relation WHERE relationTypeId='" + to_string(relationType) + "' AND parentObjectId = '"+ to_string(parent) + "'"));
    selectAction.commit();

    TopologyRelationList returnVector;
    string tmpChildName;
    TopologyObjectId tmpChild;
    TopologyRelation tmpRelation;

    // Check if there is only one result
    if (resultSelect.size() > 0) {
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            // TODO: Conversion okay?
            if (!resultSelect[i]["childObjectID"].to(tmpChild) &&
                !resultSelect[i]["childName"].to(tmpChildName)) {
                    // TODO: ERROR
            }

            Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( tmpChildName, parent, tmpChild, relationType ) );
            tmpRelation.setObject(newRelation);
            returnVector.emplace_back(tmpRelation);
        }
    }
    else {
        // Not found
    }
    return returnVector;
    
}

TopologyRelationList DatabaseTopology::enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    result resultSelect = selectAction.exec(("SELECT parentObjectID, childName FROM Relation WHERE childObjectId='" + to_string(child) + "' AND relationTypeId = '"+ to_string(relationType) + "'"));
    selectAction.commit();

    TopologyRelationList returnVector;
    string tmpChildName;
    TopologyObjectId tmpParent;
    TopologyRelation tmpRelation;

    // Check if there is only one result
    if (resultSelect.size() > 0) {
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            // TODO: Conversion okay?
            if (!resultSelect[i]["parentObjectID"].to(tmpParent) &&
                !resultSelect[i]["childName"].to(tmpChildName)) {
                    // TODO: ERROR
            }

            Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( tmpChildName, tmpParent, child, relationType ) );
            tmpRelation.setObject(newRelation);
            returnVector.emplace_back(tmpRelation);
        }
    }
    else {
        // Not found
    }
    return returnVector;
}

TopologyAttribute DatabaseTopology::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw() {

    TopologyAttribute tmp = lookupAttributeByName(domain, name);
    if(tmp){
        return tmp;
    }

    // TODO: Check if already in the database?
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "Insert Transaction");

    // Perform an insert
    insertAction.exec("INSERT INTO Attribute (name, domainTypeId, dataType) VALUES ('"+insertAction.esc(name)+"','"+to_string(domain)+"','"+to_string((intmax_t) datatype)+"')");
    insertAction.commit();

    TopologyAttribute tmpAttribute;
    Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, datatype ) );

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM Attribute WHERE name='" + selectAction.esc(name) + "' AND domainTypeId='" + to_string(domain) + "' AND dataType='"+to_string((intmax_t) datatype)+"'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        // TODO: Conversion okay?
        if (!resultSelect[0]["id"].to(newAttribute->id)) {
            // TODO: ERROR
            assert(false);
        }
    }
    else {
        // Not found
        assert(false);
    }

    tmpAttribute.setObject(newAttribute);
    return tmpAttribute;
}

TopologyAttribute DatabaseTopology::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() {
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");
    TopologyTypeId tmpId;
    intmax_t tmpInt;

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id, dataType FROM Attribute WHERE name='" + selectAction.esc(name) + "' AND domainTypeId = '"+to_string(domain)+"'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        TopologyAttribute tmpAttribute;
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            // TODO: ERROR
            assert(false);
        }
        if (!resultSelect[0]["dataType"].to(tmpInt)) {
            // TODO: ERROR
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
    work selectAction(*conn, "Select Transaction");
    string tmpName;
    //TopologyTypeId tmpId;
    int tmpId;
    intmax_t tmpInt;

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT name, domaintypeid, dataType FROM Attribute WHERE id='" + to_string(attributeId) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["name"].to(tmpName)) {
            // TODO: ERROR
            assert(false);
        }
        if (!resultSelect[0]["domaintypeid"].to(tmpId)) {
            // TODO: ERROR
            assert(false);
        }
        if (!resultSelect[0]["dataType"].to(tmpInt)) {
            // TODO: ERROR
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
    work insertAction(*conn, "Insert Transaction");
    // Perform an insert OR update the existing value
    stringstream buff;

    buff << "UPDATE Value SET value='" << value.toStr() << "' WHERE objectid=" << objectId << " AND attributeid=" << attributeId << " returning objectid"; // returning objectid is a trick to get a row back

    result resultSelect = insertAction.exec(buff.str() );
    if (resultSelect.size() == 1) {
        insertAction.commit();

        return true;
    }

    stringstream insert;
    insert << "INSERT INTO Value (objectId, attributeId, type, value) VALUES ('" << objectId << "','" << attributeId << "','" << ((int) value.type()) << "','" << value.toStr() << "')";
    insertAction.exec(insert.str() );
    insertAction.commit();

    return true;
}

TopologyValue DatabaseTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() {
    work selectAction(*conn, "Select Transaction");
    
    // Perform a select
    result resultSelect = selectAction.exec(("SELECT value, type FROM Value WHERE objectId='" + to_string(object) + "' AND attributeId = '"+to_string(attribute)+"'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        string valueString;       
        int type;

        // Check if results are sane (and convert them)
        if (!resultSelect[0]["value"].to(valueString)) {
            // TODO: ERROR
            assert(false);
        }
        if (!resultSelect[0]["type"].to(type)) {
            // TODO: ERROR
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
            // TODO: Conversion okay?
            if (!resultSelect[i]["value"].to(tmpValueString)) {
                    // TODO: ERROR
            }

            tmpValue = deserializeTopologyVariable(tmpValueString);
            returnVector.emplace_back(*tmpValue);
        }
    }
    else {
        // Not found
    }

    return returnVector;*/
    assert(0 && "TODO"), abort();
}

extern "C" {
    void* TOPOLOGY_INSTANCIATOR_NAME()
    {
        return new DatabaseTopology();
    }
}