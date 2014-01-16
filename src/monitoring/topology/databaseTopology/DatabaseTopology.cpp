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

using namespace std;
using namespace core;
using namespace monitoring;
using namespace pqxx;
using namespace boost;

class DatabaseTopology : public Topology {
    public:
        DatabaseTopology();
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
        virtual TopologyValue setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyVariable& value ) throw();
        virtual TopologyValue getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw();
        virtual TopologyValueList enumerateAttributes( TopologyObjectId object ) throw();

    private:
        connection* conn;

        string serializeTopologyVariable(TopologyVariable value) {
            /* OLD AND BROKEN
            ONLY HERE FOR REFERENCE
            string result = to_string(value->object)+"|"+to_string(value->attribute);
            int tmpint = (int8_t) value->value.type();
            result = result+"|"+to_string(tmpint)+"|"+value->value.str();
            return result;
            */

            string result = to_string(value.str());
            int tmpint = (int8_t) value.type();
            result = result+"|"+to_string(tmpint);

            return result;
        }

        TopologyVariable* deserializeTopologyVariable(string value) {
            /*OLD AND BROKEN
            ONLY HERE FOR REFERENCE
            TopologyValue tmpValue;
            TopologyObjectId tmpObject;
            TopologyAttributeId tmpAttr;
            int8_t tmpValueInt;
            TopologyVariable *innerValue;
            
            vector<string> SplitVec;
            split(SplitVec, value, is_any_of("|"), token_compress_on);

            tmpObject = stoul(SplitVec.at(0));
            tmpAttr = stoul(SplitVec.at(1));
            innerValue = new VariableDatatype(SplitVec.at(2), (int8_t) stoul(SplitVec.at(3)));

            Release<TopologyValueImplementation> newValue(new TopologyValueImplementation(*innerValue, tmpObject, tmpAttr));
            tmpValue.setObject(newValue);

            return tmpValue;*/

            TopologyVariable *innerValue;

            vector<string> SplitVec;
            split(SplitVec, value, is_any_of("|"), token_compress_on);

            //innerValue = new VariableDatatype(SplitVec.at(0), (int8_t) stoul(SplitVec.at(1)));

            return innerValue;
        }
};

DatabaseTopology::DatabaseTopology() {
    // TODO: Alles in init?
}

DatabaseTopology::~DatabaseTopology() {
    conn->disconnect();
}

void DatabaseTopology::init() {
    conn = new connection("hostaddr=127.0.0.1 port=5432 user=postgres password=unitek dbname=siox");
}

ComponentOptions* DatabaseTopology::AvailableOptions() {
    return new DatabaseTopologyOptions;
}

TopologyType DatabaseTopology::registerType( const string& name ) throw() {
    TopologyType tmpType;
    Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name));

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
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(newType->id)) {
            // TODO: ERROR
        }
    }
    else {
        // TODO: ERROR
    }

    tmpType.setObject(newType);
    return tmpType;
}

TopologyType DatabaseTopology::lookupTypeByName( const string& name ) throw() {
    TopologyType tmpType;
    Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name));

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM Type WHERE name='" + selectAction.esc(name) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(newType->id)) {
            // TODO: ERROR
        }
        tmpType.setObject(newType);
    }
    else {
        // Not found
    }

    return tmpType;
}

TopologyType DatabaseTopology::lookupTypeById( TopologyTypeId anId ) throw() {
    TopologyType tmpType;
    string tmpName;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec("SELECT name FROM Type WHERE id='"+to_string(anId)+"'");
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["name"].to(tmpName)) {
            // TODO: ERROR
        }
        Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(tmpName));
        newType->id = anId;
        tmpType.setObject(newType);
    }
    else {
        // Not found
    }

    return tmpType;
}

TopologyObject DatabaseTopology::registerObject( TopologyObjectId parentId, TopologyTypeId relationType, const string& childName, TopologyTypeId objectType ) throw() {
    TopologyObject tmpObject;
    Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation( objectType ) );

    // TODO: Check if already in the database?
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "Insert Transaction");

    // Perform the insert
    insertAction.exec("INSERT INTO Object (typeId) VALUES ('"+to_string(objectType)+"')");
    insertAction.commit();

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM Object WHERE typeId='" + to_string(objectType) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        // TODO: Conversion okay?
        if (!resultSelect[0]["id"].to(newObject->id)) {
            // TODO: ERROR
        }
        tmpObject.setObject(newObject);
    }
    else {
        // TODO: ERROR
    }

    // Perform another insert for the relation
    work insertAction2(*conn, "Insert Transaction");
    insertAction2.exec("INSERT INTO Relation (parentObjectId, childName, childObjectId, relationTypeId) VALUES ('"+to_string(parentId)+"','"+insertAction2.esc(childName)+"','"+to_string(newObject->id)+"','"+to_string(relationType)+"')");
    insertAction2.commit();

    return tmpObject;
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
        }
        Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(tmpId) );
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

    // Perform another insert
    insertAction.exec("INSERT INTO Relation (parentObjectId, childName, childObjectId, relationTypeId) VALUES ('"+to_string(parent)+"','"+insertAction.esc(childName)+"','"+to_string(child)+"','"+to_string(relationType)+"')");
    insertAction.commit();

    Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );

    tmpRelation.setObject(newRelation);
    return tmpRelation;
}

TopologyRelation DatabaseTopology::lookupRelation( TopologyObjectId parent, TopologyTypeId relationType, const string& childName ) throw() {
    TopologyRelation tmpRelation;

    TopologyTypeId tmpRelationType;
    TopologyObjectId tmpChild;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT childObjectID, relationTypeId FROM Relation WHERE childName='" + selectAction.esc(childName) + "' AND parentObjectId = '"+ to_string(parent) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        // TODO: Conversion okay?
        if (!resultSelect[0]["childObjectID"].to(tmpChild)) {
            // TODO: ERROR
        }
        if (!resultSelect[0]["relationTypeId"].to(tmpRelationType)) {
            // TODO: ERROR
        }
        Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, tmpChild, tmpRelationType ) );
        tmpRelation.setObject(newRelation);
    }
    else {
        // Not found
    }

    return tmpRelation;
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
        }
    }
    else {
        // Not found
    }

    tmpAttribute.setObject(newAttribute);
    return tmpAttribute;
}

TopologyAttribute DatabaseTopology::lookupAttributeByName( TopologyTypeId domain, const string& name ) throw() {
    TopologyAttribute tmpAttribute;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");
    TopologyTypeId tmpId;
    intmax_t tmpInt;

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id, dataType FROM Attribute WHERE name='" + selectAction.esc(name) + "' AND domainTypeId = '"+to_string(domain)+"'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        if (!resultSelect[0]["id"].to(tmpId)) {
            // TODO: ERROR
        }
        if (!resultSelect[0]["dataType"].to(tmpInt)) {
            // TODO: ERROR
        }
        VariableDatatype::Type tmpType = (VariableDatatype::Type) tmpInt;
        Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, tmpType ) );
        newAttribute->id = tmpId;
        tmpAttribute.setObject(newAttribute);    
    }
    else {
        // Not found
    }

    return tmpAttribute;
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
        }
        if (!resultSelect[0]["domaintypeid"].to(tmpId)) {
            // TODO: ERROR
        }
        if (!resultSelect[0]["dataType"].to(tmpInt)) {
            // TODO: ERROR
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

TopologyValue DatabaseTopology::setAttribute( TopologyObjectId object, TopologyAttributeId attributeId, const TopologyVariable& value ) throw() {
    work insertAction(*conn, "Insert Transaction");

    string insertType = serializeTopologyVariable(value);

    // Perform an insert
    insertAction.exec("INSERT INTO Value (objectId, attributeId, topologyvalue) VALUES ('"+to_string(object)+"','"+to_string(attributeId)+"','"+insertType+"')");
    insertAction.commit();

    TopologyValue value1;
    Release<TopologyValueImplementation> newValue( new TopologyValueImplementation( value, object, attributeId ) );
    value1.setObject(newValue);

    return value1;
}

TopologyValue DatabaseTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() {
    work selectAction(*conn, "Select Transaction");
    string valueString;
    TopologyValue value;
    
    // Perform a select
    result resultSelect = selectAction.exec(("SELECT value FROM Value WHERE objectId='" + to_string(object) + "' AND attributeId = '"+to_string(attribute)+"'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        // TODO: Conversion okay?
        if (!resultSelect[0]["value"].to(valueString)) {
                // TODO: ERROR
        }
        TopologyVariable *tmpVarib = deserializeTopologyVariable(valueString);
    
        Release<TopologyValueImplementation> newValue( new TopologyValueImplementation( *tmpVarib, object, attribute ) );
        value.setObject(newValue);
    }
    else {
        // Not found
    }

    return value;
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