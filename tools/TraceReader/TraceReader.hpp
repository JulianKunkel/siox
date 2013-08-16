#ifndef TOOLS_TRACE_READER_H
#define TOOLS_TRACE_READER_H

#include <string>
#include <sstream>

#include <core/container/container-serializer-text.hpp>

#include <monitoring/association_mapper/AssociationMapper.hpp>
#include <monitoring/ontology/Ontology.hpp>
#include <monitoring/system_information/SystemInformationGlobalIDManager.hpp>

#include <monitoring/datatypes/ActivitySerializable.hpp>

using namespace std;
using namespace monitoring;
using namespace core;


class TraceReader{
public:
	TraceReader(string activityFile, string systemInfoFile, string ontologyFile, string associationFile);

	Activity * nextActivity(){
		if (activityDeserializer->hasNext())
			return activityDeserializer->parseNext();
		else
			return nullptr;
	}

	void printActivity(Activity * activity);
private:
	AssociationMapper * a;
	Ontology * o;
	SystemInformationGlobalIDManager * s;
	FileDeserializer<ActivitySerializable> * activityDeserializer;

	void strattribute(const Attribute & a, stringstream & s) throw(NotFoundError);
};

#endif