#ifndef CORE_COMPONENT_REPORTINTERFACE_HPP
#define CORE_COMPONENT_REPORTINTERFACE_HPP

#include <unordered_map>
#include <string>
#include <memory>

#include <core/datatypes/VariableDatatype.hpp>

using namespace std;

namespace core{

/*
 A single information gathered which may or may not reported based on the RuntimeReporter and EntryType
 */
struct ReportEntry{
	enum class Type : int8_t {
		APPLICATION, // some information about the application
		SYSTEM, 
		SIOX_INTERNAL_P1, // SIOX internal information, highest priority (critical)
		SIOX_INTERNAL_P2, // SIOX internal information
		SIOX_INTERNAL_P3, // SIOX internal information
	};

	Type type;

	VariableDatatype value;

	ReportEntry(const Type type, const VariableDatatype & value) : type(type), value(value) {}
	ReportEntry(){}
};

struct ComponentReport{
	string componentType;
	string componentIdentification;

	// relevant data to report
	unordered_map<string, ReportEntry> data;

	ComponentReport(const string & cType, const string & cID): componentType(cType), componentIdentification(cID) {}
};

/**
 Implement this interface on a component to offer RuntimeInformation.
 */
class ComponentReportInterface{
	public:
		virtual shared_ptr<ComponentReport> prepareReport() = 0;
};

}

#endif