#ifndef __ASSOCIATION_MAPPER_H
#define __ASSOCIATION_MAPPER_H

#include <core/component/Component.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

namespace monitoring {



	class AssociationMapper : public core::Component {
		public:
			/*
			Domain: "instance"
			Key: String, Value int32_t

			Domain: "process"
			OntologyAttributeID
			Beliebiger Value_Type. bzw. given by attribute

			Domain "component"
			Key: cid + OntologyAttributeID, VALUE as value...
			*/
			virtual void set_process_attribute( const ProcessID & pid, const OntologyAttribute & att, const OntologyValue & value ) throw( IllegalStateError ) = 0;
			virtual const OntologyValue lookup_process_attribute( const ProcessID & pid, const OntologyAttribute & att ) const throw( NotFoundError ) = 0;

			virtual void set_component_attribute( const ComponentID & cid, const OntologyAttribute & att, const  OntologyValue & value ) throw( IllegalStateError ) = 0;
			virtual const OntologyValue lookup_component_attribute( const ComponentID & cid, const OntologyAttribute & att ) const throw( NotFoundError ) = 0;

			/* These functions are used to create the RemoteInstanceID */
			virtual AssociateID create_instance_mapping( const string & value ) = 0;
			virtual const string lookup_instance_mapping( AssociateID id ) const throw( NotFoundError ) = 0;

			string localHostname(){
				return hostname;
			}

			ProcessID localProcessID(){
				return processID;
			}

			void setLocalInformation(const string & hostname, const ProcessID & processID){
				this->processID = processID;
				this->hostname = hostname;
			}

		private:
			ProcessID processID;
			string hostname = "undefined";		
	};

}

#define MONITORING_ASSOCIATION_MAPPER_INTERFACE "monitoring_association_mapper"

#endif

// BUILD_TEST_INTERFACE monitoring/association_mapper/modules/
