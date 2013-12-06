/**
 * A Topology implementation that keeps all data in main memory.
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#include <util/ExceptionHandling.hpp>
#include <monitoring/topology/TopologyImplementation.hpp>

#include <unordered_map>
#include <boost/thread/shared_mutex.hpp>

#include "RamTopologyOptions.hpp"

using namespace std;
using namespace core;
using namespace monitoring;

using TopologyValue = Topology::TopologyValue;
using Type = Topology::Type;
using TypeId = Topology::TypeId;
using Object = Topology::Object;
using ObjectId = Topology::ObjectId;
using Relation = Topology::Relation;
using RelationId = Topology::RelationId;
using Value = Topology::Value;
using ValueId = Topology::ValueId;
using Attribute = Topology::Attribute;
using AttributeId = Topology::AttributeId;


class RamTopology : public Topology {
	public:
		virtual void init();
		virtual ComponentOptions * AvailableOptions();

		virtual const Type& registerType( const string& name ) throw();
		virtual const Type& lookupTypeByName( const string& name ) throw( NotFoundError );
		virtual const Type& lookupTypeById( TypeId anId ) throw( NotFoundError );

		virtual const Object& registerObject( ObjectId parent, TypeId objectType, TypeId relationType, const string& childName ) throw( IllegalStateError );
		virtual const Object& lookupObjectByPath( const string& Path ) throw( NotFoundError );
		virtual const Object& lookupObjectById( Object anId ) throw( NotFoundError );	//Passing 0 yields a reference to the root object.

		virtual const Relation& registerRelation( TypeId relationType, ObjectId parent, ObjectId child, const string& childName ) throw( IllegalStateError );
		virtual const Relation& lookupRelation( ObjectId parent, const string& childName ) throw( NotFoundError );

		// RelationType may be 0 which indicates any parent/child.
		virtual std::vector<const Relation*> enumerateChildren( ObjectId parent, TypeId relationType ) throw();
		virtual std::vector<const Relation*> enumerateParents( ObjectId child, TypeId relationType ) throw();


		virtual const Attribute& registerAttribute( TypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError );
		virtual const Attribute& lookupAttributeByName( TypeId domain, const string& name ) throw( NotFoundError );
		virtual const Attribute& lookupAttributeById( AttributeId attributeId ) throw( NotFoundError );
		virtual void setAttribute( ObjectId object, AttributeId attribute, const TopologyValue& value ) throw( IllegalStateError );
		virtual const TopologyValue& getAttribute( ObjectId object, AttributeId attribute ) throw( NotFoundError );
		virtual std::vector<const Value*> enumerateAttributes( ObjectId object ) throw();

	private:
		boost::shared_mutex typesLock;
		unordered_map<string, const Type*> typesByName;	//Protected by typesLock.
		vector<const Type*> typesById;	//Protected by typesLock. The IDs are 1 based, the index into this vector is not!
};


void RamTopology::init() {
}


ComponentOptions* RamTopology::AvailableOptions() {
	return new RamTopologyOptions;
}


const Type& RamTopology::registerType( const string& name ) throw() {
	const Type* result = 0;
	typesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = typesByName.at( name ); );
	typesLock.unlock_shared();
	if( !result ) {
		Type* newType = new Type{ .name = name, .id = 0 };	//The new operator can take a _lot_ of time, don't let other processes wait for that!

		typesLock.lock();
		IGNORE_EXCEPTIONS( result = typesByName.at( name ); );	//Someone else might have registered it in the mean time.
		if( !result ) {
			newType->id = typesById.size() + 1;
			typesByName[name] = newType;
			typesById.emplace_back( newType );
			result = newType;
			newType = NULL;
		}
		typesLock.unlock();

		delete newType;
	}
	return *result;
}


const Type& RamTopology::lookupTypeByName( const string& name ) throw( NotFoundError ) {
	const Type* result = 0;
	typesLock.lock_shared();
	IGNORE_EXCEPTIONS( result = typesByName.at( name ); );
	typesLock.unlock_shared();
	return *result;
}


const Type& RamTopology::lookupTypeById( TypeId anId ) throw( NotFoundError ) {
	if( !anId ) throw NotFoundError();
	const Type* result = 0;
	typesLock.lock_shared();
	if( anId - 1 < typesById.size() ) result = typesById[anId - 1];
	typesLock.unlock_shared();
	return *result;
}

const Object& RamTopology::registerObject( ObjectId parent, TypeId objectType, TypeId relationType, const string& childName ) throw( IllegalStateError ) {
	assert(0 && "TODO"), abort();
}

const Object& RamTopology::lookupObjectByPath( const string& Path ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}

const Object& RamTopology::lookupObjectById( Object anId ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}

const Relation& RamTopology::registerRelation( TypeId relationType, ObjectId parent, ObjectId child, const string& childName ) throw( IllegalStateError ) {
	assert(0 && "TODO"), abort();
}

const Relation& RamTopology::lookupRelation( ObjectId parent, const string& childName ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}

std::vector<const Relation*> RamTopology::enumerateChildren( ObjectId parent, TypeId relationType ) throw() {
	assert(0 && "TODO"), abort();
}

std::vector<const Relation*> RamTopology::enumerateParents( ObjectId child, TypeId relationType ) throw() {
	assert(0 && "TODO"), abort();
}

const Attribute& RamTopology::registerAttribute( TypeId domain, const string& name, VariableDatatype::Type datatype ) throw( IllegalStateError ) {
	assert(0 && "TODO"), abort();
}

const Attribute& RamTopology::lookupAttributeByName( TypeId domain, const string& name ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}

const Attribute& RamTopology::lookupAttributeById( AttributeId attributeId ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}

void RamTopology::setAttribute( ObjectId object, AttributeId attribute, const TopologyValue& value ) throw( IllegalStateError ) {
	assert(0 && "TODO"), abort();
}

const TopologyValue& RamTopology::getAttribute( ObjectId object, AttributeId attribute ) throw( NotFoundError ) {
	assert(0 && "TODO"), abort();
}

std::vector<const Value*> RamTopology::enumerateAttributes( ObjectId object ) throw() {
	assert(0 && "TODO"), abort();
}


extern "C" {
	void* TOPOLOGY_INSTANCIATOR_NAME()
	{
		return new RamTopology();
	}
}
