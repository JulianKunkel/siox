#ifndef TOOLS_TRACE_READER_H
#define TOOLS_TRACE_READER_H

#include <string>
#include <sstream>

//#include <core/container/container-serializer-text.hpp>

#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivitySerializationPlugin.hpp>
#include <monitoring/topology/Topology.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>

using namespace std;
using namespace monitoring;
using namespace core;


class TraceReader {
	public:
		TraceReader( string activityFile, string systemInfoFile, string ontologyFile, string associationFile, string topologyDatabase, string activityReader );

		std::shared_ptr<Activity> nextActivity();

		void printActivity( std::shared_ptr<Activity> activity );

		bool printHostname = false;

		Ontology * getOntology() const{
			return o;
		}

		AssociationMapper * getAssociationMapper() const{
			return a;
		}

		SystemInformationGlobalIDManager * getSystemInformationGlobalIDManager() const{
			return s;
		}

		~TraceReader(){
			activityDeserializer->closeTrace();
			s->stop();
			o->stop();
			s->finalize();
			o->finalize();
			delete(s);
			delete(o);
			if (t){
				t->stop();
				t->finalize();
				delete(t);
			}			
		}
	private:
		AssociationMapper * a = nullptr;
		Ontology * o = nullptr;
		SystemInformationGlobalIDManager * s = nullptr;
		Topology * t = nullptr;
		ActivitySerializationPlugin * activityDeserializer = nullptr;

		void strattribute( const Attribute & a, stringstream & s ) throw( NotFoundError );
};

#endif
