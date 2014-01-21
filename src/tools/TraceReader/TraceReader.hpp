#ifndef TOOLS_TRACE_READER_H
#define TOOLS_TRACE_READER_H

#include <string>
#include <sstream>

#include <core/container/container-serializer-text.hpp>

#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>
#include <monitoring/datatypes/Activity.hpp>

using namespace std;
using namespace monitoring;
using namespace core;


class TraceReader {
	public:
		TraceReader( string activityFile, string systemInfoFile, string ontologyFile, string associationFile );

		Activity * nextActivity();

		void printActivity( Activity * activity );

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
	private:
		AssociationMapper * a;
		Ontology * o;
		SystemInformationGlobalIDManager * s;
		FileDeserializer<Activity> * activityDeserializer;

		void strattribute( const Attribute & a, stringstream & s ) throw( NotFoundError );
};

#endif