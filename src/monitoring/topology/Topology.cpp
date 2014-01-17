#include <monitoring/topology/Topology.hpp>

// types for pure string handling
#define TOPO_PATH_CHAR '/'
#define TOPO_ALIAS_CHAR '@'
#define TOPO_TYPE_CHAR ':'


namespace monitoring{
	TopologyObject Topology::registerObjectByPath( const vector<CreateObjectPathElement> & path, TopologyObjectId parent ) throw(){
		TopologyObjectId resultId = parent;
		TopologyObject result;
		for( size_t i = 0; i < path.size(); i++ ) {
			const CreateObjectPathElement & curComponent = path[i];
			TopologyType relationType = registerType( curComponent.relationName );
			assert( relationType );
			TopologyType childType = registerType( curComponent.objectType );
			assert( childType );
			result = registerObject( resultId, relationType.id(), curComponent.name, childType.id() );
			if( ! result ) return TopologyObject();
		}
		return result;
	}

	TopologyObject Topology::lookupObjectByPath( const vector<ObjectPathElement> & path, TopologyObjectId parent ) throw(){
		TopologyObjectId resultId = parent;
		TopologyObject result;
		for( size_t i = 0; i < path.size(); i++ ) {
			const ObjectPathElement & curComponent = path[i];
			//Lookup the corresponding relation and update resultId.
			TopologyType relationType = lookupTypeByName( curComponent.relationName );
			if( !relationType ) return TopologyObject();
			TopologyRelation relation = lookupRelation( resultId, relationType.id(), curComponent.name );
			if( !relation )  return TopologyObject();
			resultId = relation.child();
		}
		return lookupObjectById( resultId );
	}

	Topology::PathComponentDescription* Topology::parsePath( const string& path, size_t* outComponentCount ) throw() {
		//First count the path components, so that we can allocate descriptions for them.
		size_t componentCount = 1, pathSize = path.size();
		if( !pathSize ) return NULL;
		for( size_t i = pathSize; i--; ) if( path[i] == TOPO_PATH_CHAR ) componentCount++;
		//Get some mem.
		PathComponentDescription* result = new PathComponentDescription[componentCount];
		//Parse the components.
		for( size_t i = 0, curComponent = 0; i < pathSize; i++, curComponent++ ) {
			size_t componentStart = i;
			for( ; i < pathSize && path[i] != TOPO_PATH_CHAR; i++ ) ;
			string componentString = path.substr( componentStart, i - componentStart );
			if( !parsePathComponent( componentString, &result[curComponent] ) ) {
				delete[] result;
				return NULL;
			}
		}
		*outComponentCount = componentCount;
		return result;
	}

	bool Topology::parsePathComponent( const string& aliasedComponent, Topology::PathComponentDescription* outDescription ) throw() {
		//Substitute possible aliases.
		const string* component = &aliasedComponent;
		aliasesLock.lock_shared();
		{
			while( component && component->size() && (*component)[0] == TOPO_ALIAS_CHAR ) {
				const string* temp = NULL;
				IGNORE_EXCEPTIONS( temp = &aliases.at( *component ); );
				component = temp;
			}
		}
		aliasesLock.unlock_shared();
		if( !component ) return false;
		//Sanity check: Count the number of colons in the component string.
		size_t colonCount = 0, componentSize = component->size();
		for( size_t i = 0; i < componentSize; i++) {
			switch( (*component)[i] ) {
				case TOPO_TYPE_CHAR: colonCount++; break;
				case TOPO_PATH_CHAR: return false;
			}
		}
		if( !colonCount || colonCount > 2 ) return false;
		if( colonCount == 1 ) {
			//Find the one colon and check for empty names.
			size_t colonPosition = 0;
			for( ; (*component)[colonPosition] != TOPO_TYPE_CHAR; colonPosition++ ) ;
			if( !colonPosition || colonPosition == componentSize - 1 ) return false;
			//Fill in the descriptor.
			outDescription->relationName = component->substr( 0, colonPosition );
			outDescription->childName = component->substr( colonPosition + 1, componentSize - colonPosition - 1 );
			outDescription->haveChildType = false;
		} else {
			//Find the colon positions and check for empty names.
			size_t colon1 = 0, colon2 = 0;
			for( ; (*component)[colon1] != TOPO_TYPE_CHAR; colon1++ ) ;
			for( colon2 = colon1 + 1; (*component)[colon2] != TOPO_TYPE_CHAR; colon2++ ) ;
			if( !colon1 || colon1 == colon2 - 1 || colon2 == componentSize - 1 ) return false;
			//Fill in the descriptor.
			outDescription->relationName = component->substr( 0, colon1 );
			outDescription->childName = component->substr( colon1 + 1, colon2 - colon1 - 1 );
			outDescription->childTypeName = component->substr( colon2 + 1, componentSize - colon2 - 1 );
			outDescription->haveChildType = true;
		}
		return true;
	}

	TopologyObject Topology::registerObjectByPath( const string& path, TopologyObjectId parent ) throw() {
		size_t componentCount;
		if( PathComponentDescription* components = parsePath( path, &componentCount ) ) {
			TopologyObjectId resultId = parent;
			TopologyObject result;
			for( size_t i = 0; i < componentCount; i++ ) {
				PathComponentDescription* curComponent = &components[i];
				//Lookup the corresponding relation and update resultId.
				TopologyType relationType = registerType( curComponent->relationName );
				assert( relationType );
				if( !curComponent->haveChildType ) curComponent->childTypeName = curComponent->relationName;
				TopologyType childType = registerType( curComponent->childTypeName );
				assert( childType );
				TopologyObject child = registerObject( resultId, relationType.id(), curComponent->childName, childType.id() );
				if( !child ) break;
				resultId = child.id();
				if( i == componentCount - 1 ) result = child;
			}
			delete[] components;
			return result;
		}
		return TopologyObject();
	}

	TopologyObject Topology::lookupObjectByPath( const string& path, TopologyObjectId parent ) throw() {
		size_t componentCount;
		if( PathComponentDescription* components = parsePath( path, &componentCount ) ) {
			TopologyObjectId resultId = parent;
			TopologyObject result;
			for( size_t i = 0; i < componentCount; i++ ) {
				PathComponentDescription* curComponent = &components[i];
				//Lookup the corresponding relation and update resultId.
				TopologyType relationType = lookupTypeByName( curComponent->relationName );
				if( !relationType ) break;
				TopologyRelation relation = lookupRelation( resultId, relationType.id(), curComponent->childName );
				if( !relation ) break;
				resultId = relation.child();
				if( curComponent->haveChildType ) {	//Does the user want a type check?
					TopologyObject child = lookupObjectById( resultId );
					TopologyType expectedType = lookupTypeByName( curComponent->childTypeName );
					if( !child || !expectedType || child.type() != expectedType.id() ) break;
					if( i == componentCount - 1 ) result = child;
				} else if( i == componentCount - 1 ) {
					result = lookupObjectById( resultId );
				}
			}
			delete[] components;
			return result;
		}
		return TopologyObject();
	}

	bool Topology::setAlias( const string& name, const string& value ) throw() {
		//First some sanity checks.
		size_t nameSize = name.size();
		if( nameSize < 2 ) return false;
		if( name[0] != '@' ) return false;
		
		for( size_t i = nameSize; i--; ) if( name[i] == TOPO_PATH_CHAR ) return false;
		PathComponentDescription trash;
		if( !parsePathComponent( value, &trash ) ) return false;

		bool success = false;
		aliasesLock.lock();
		{
			string* oldValue = NULL;
			IGNORE_EXCEPTIONS( oldValue = &aliases.at( name ); );
			if( !oldValue || *oldValue == value ) {
				//Then the actual "work".
				aliases[name] = value;
				success = true;
			}
		}
		aliasesLock.unlock();
		return success;
	}
}
