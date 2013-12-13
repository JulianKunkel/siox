/**
 * @author Roman Michel
 * @date 2013
 */

#include <util/ExceptionHandling.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>
#include <workarounds.hpp>
 #include <boost/algorithm/string.hpp>

 #include <pqxx/pqxx>
 #include <boost/algorithm/string.hpp>

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

        virtual TopologyType registerType( const string& name ) throw();
        virtual TopologyType lookupTypeByName( const string& name ) throw();
        virtual TopologyType lookupTypeById( TopologyTypeId anId ) throw();

        virtual TopologyObject registerObject( TopologyObjectId parent, TopologyTypeId objectType, TopologyTypeId relationType, const string& childName ) throw( IllegalStateError );
        virtual TopologyObject lookupObjectByPath( const string& Path ) throw();
        virtual TopologyObject lookupObjectById( TopologyObjectId anId ) throw();

        virtual TopologyRelation registerRelation( TopologyTypeId relationType, TopologyObjectId parent, TopologyObjectId child, const string& childName ) throw( IllegalStateError );
        virtual TopologyRelation lookupRelation( TopologyObjectId parent, const string& childName ) throw();

        virtual TopologyRelationList enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw();
        virtual TopologyRelationList enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw();


        virtual TopologyAttribute registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError );
        virtual TopologyAttribute lookupAttributeByName( TopologyTypeId domain, const string& name ) throw();
        virtual TopologyAttribute lookupAttributeById( TopologyAttributeId attributeId ) throw();
        virtual void setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyValue& value ) throw( IllegalStateError );
        virtual TopologyValue getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw();
        virtual TopologyValueList enumerateAttributes( TopologyObjectId object ) throw();

    private:
        connection* conn;

        string serializeTopologyValue(TopologyValue value) {
            string result = to_string(value->object)+"|"+to_string(value->attribute);
            int tmpint = (int8_t) value->value.type();
            result = result+"|"+to_string(tmpint)+"|"+value->value.str();
            return result;
        }

        TopologyValue& deserializeTopologyValue(string value) {
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
        }
};

DatabaseTopology::DatabaseTopology() {
    // TODO: Alles in init?
}

DatabaseTopology::~DatabaseTopology() {
    conn->disconnect();
}

void DatabaseTopology::init() {
    conn = new connection("hostaddr=136.172.14.14 port=48142 user=siox password=xxx dbname=siox");
}

TopologyType DatabaseTopology::registerType( const string& name ) throw() {
    TopologyType tmpType;
    Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(name));

    // TODO: Check if already in the database?
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "Insert Transaction");

    // Perform the insert
    insertAction.exec("INSERT INTO Type (name) VALUES ('"+insertAction.esc(name)+"')");
    insertAction.commit();

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT id FROM Type WHERE name='" + selectAction.esc(name) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        // TODO: Conversion okay?
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
        // TODO: Conversion okay?
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
        // TODO: Conversion okay?
        if (!resultSelect[0]["name"].to(tmpName)) {
            // TODO: ERROR
        }
    }
    else {
        // TODO: ERROR
    }       

    Release<TopologyTypeImplementation> newType(new TopologyTypeImplementation(tmpName));
    newType->id = anId;
    tmpType.setObject(newType);

    return tmpType;
}

TopologyObject DatabaseTopology::registerObject( TopologyObjectId parentId, TopologyTypeId objectType, TopologyTypeId relationType, const string& childName ) throw( IllegalStateError ) {
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
    }
    else {
        // TODO: ERROR
    }       

    // Perform another insert for the relation
    insertAction.exec("INSERT INTO Relation (parentObjectId, childName, childObjectId, relationTypeId) VALUES ('"+to_string(parentId)+"','"+insertAction.esc(childName)+"','"+to_string(newObject->id)+"','"+to_string(relationType)+"')");
    insertAction.commit();

    tmpObject.setObject(newObject);
    return tmpObject;
}

TopologyObject DatabaseTopology::lookupObjectByPath( const string& path ) throw() {
    // TODO: Old logic completly broken. Rethink.
    assert(0 && "TODO"), abort();
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
        // TODO: Conversion okay?
        if (!resultSelect[0]["TypeId"].to(tmpId)) {
            // TODO: ERROR
        }
    }
    else {
        // TODO: ERROR
    }       

    Release<TopologyObjectImplementation> newObject( new TopologyObjectImplementation(tmpId) );
    tmpObject.setObject(newObject);
    return tmpObject;
}

TopologyRelation DatabaseTopology::registerRelation( TopologyTypeId relationType, TopologyObjectId parent, TopologyObjectId child, const string& childName ) throw( IllegalStateError ) {
    TopologyRelation tmpRelation;
    Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, child, relationType ) );

    // TODO: Check if already in the database?
    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work insertAction(*conn, "Insert Transaction");

    // Perform another insert
    insertAction.exec("INSERT INTO Relation (parentObjectId, childName, childObjectId, relationTypeId) VALUES ('"+to_string(parent)+"','"+insertAction.esc(childName)+"','"+to_string(child)+"','"+to_string(relationType)+"')");
    insertAction.commit();

    tmpRelation.setObject(newRelation);
    return tmpRelation;
}

TopologyRelation DatabaseTopology::lookupRelation( TopologyObjectId parent, const string& childName ) throw() {
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
    }
    else {
        // TODO: ERROR
    }       

    Release<TopologyRelationImplementation> newRelation( new TopologyRelationImplementation( childName, parent, tmpChild, tmpRelationType ) );
    tmpRelation.setObject(newRelation);
    return tmpRelation;
}

Topology::TopologyRelationList DatabaseTopology::enumerateChildren( TopologyObjectId parent, TopologyTypeId relationType ) throw() {
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
        // TODO: ERROR
    }
    return returnVector;
    
}

Topology::TopologyRelationList DatabaseTopology::enumerateParents( TopologyObjectId child, TopologyTypeId relationType ) throw() {
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
        // TODO: ERROR
    }
    return returnVector;
}

TopologyAttribute DatabaseTopology::registerAttribute( TopologyTypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError ) {
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
        // TODO: ERROR
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
        // TODO: Conversion okay?
        if (!resultSelect[0]["id"].to(tmpId) &&
            !resultSelect[0]["dataType"].to(tmpInt)) {
                // TODO: ERROR
        }
    }
    else {
        // TODO: ERROR
    }       

    // TODO: Geht der Cast so?
    VariableDatatype::Type tmpType = (VariableDatatype::Type) tmpInt;
    Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( name, domain, tmpType ) );
    newAttribute->id = tmpId;
    tmpAttribute.setObject(newAttribute);

    return tmpAttribute;
}

TopologyAttribute DatabaseTopology::lookupAttributeById( TopologyAttributeId attributeId ) throw() {
    TopologyAttribute tmpAttribute;

    // Create a new transaction. It gets automatically destroyed at the end of this funtion.
    work selectAction(*conn, "Select Transaction");
    string tmpName;
    TopologyTypeId tmpId;
    intmax_t tmpInt;

    // Perform a select
    result resultSelect = selectAction.exec(("SELECT name, domainTypeId, dataType FROM Attribute WHERE id='" + to_string(attributeId) + "'"));
    selectAction.commit();

    // Check if there is only one result
    if (resultSelect.size() == 1) {
        // Check if results are sane (and convert them)
        // TODO: Conversion okay?
        if (!resultSelect[0]["name"].to(tmpName) &&
            !resultSelect[0]["domainTypeId"].to(tmpId) &&
            !resultSelect[0]["dataType"].to(tmpInt)) {
                // TODO: ERROR
        }
    }
    else {
        // TODO: ERROR
    }

    // TODO: Geht der Cast so?
    VariableDatatype::Type tmpType = (VariableDatatype::Type) tmpInt;
    Release<TopologyAttributeImplementation> newAttribute( new TopologyAttributeImplementation( tmpName, tmpId, tmpType ) );
    newAttribute->id = attributeId;
    tmpAttribute.setObject(newAttribute);

    return tmpAttribute;
}

void DatabaseTopology::setAttribute( TopologyObjectId object, TopologyAttributeId attribute, const TopologyValue& value ) throw( IllegalStateError ) {
    work insertAction(*conn, "Insert Transaction");

    string insertType = serializeTopologyValue(value);

    // Perform an insert
    insertAction.exec("INSERT INTO Value (objectId, attributeId, value) VALUES ('"+to_string(object)+"','"+to_string(attribute)+"','"+insertType+"')");
    insertAction.commit();

    return;
}

TopologyValue DatabaseTopology::getAttribute( TopologyObjectId object, TopologyAttributeId attribute ) throw() {
    work selectAction(*conn, "Select Transaction");
    string valueString;
    
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
    }
    else {
        // TODO: ERROR
    }

    return deserializeTopologyValue(valueString);
}

Topology::TopologyValueList DatabaseTopology::enumerateAttributes( TopologyObjectId object ) throw() {
    work selectAction(*conn, "Select Transaction");
    string valueString;
    
    // Perform a select
    result resultSelect = selectAction.exec(("SELECT value FROM Value WHERE objectId='" + to_string(object) + "'"));
    selectAction.commit();

    string tmpValueString;
    TopologyValue tmpValue;
    Topology::TopologyValueList returnVector;

    // Check if there is only one result
    if (resultSelect.size() > 0) {
        for (result::size_type i = 0; i != resultSelect.size(); ++i) {
            // TODO: Conversion okay?
            if (!resultSelect[i]["value"].to(tmpValueString)) {
                    // TODO: ERROR
            }

            tmpValue = deserializeTopologyValue(tmpValueString);
            returnVector.emplace_back(tmpValue);
        }
    }
    else {
        // TODO: ERROR
    }

    return returnVector;
}
