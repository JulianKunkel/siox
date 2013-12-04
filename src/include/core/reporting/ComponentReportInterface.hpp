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
		APPLICATION_CRITICAL, // some information about the application
		APPLICATION_INFO,
		APPLICATION_DEBUG,
		APPLICATION_PERFORMANCE,
		SYSTEM_CRITICAL, 
		SYSTEM_INFO, 
		SYSTEM_DEBUG,
		SYSTEM_PERFORMANCE,
		SIOX_INTERNAL_CRITICAL, // SIOX internal information, highest priority (critical)
		SIOX_INTERNAL_INFO,
		SIOX_INTERNAL_DEBUG,
		SIOX_INTERNAL_PERFORMANCE,
	};

	Type type;
	uint8_t priority;

	VariableDatatype value;

	ReportEntry(const Type type, const VariableDatatype & value, uint8_t priority = 1) : type(type), priority(priority), value(value) {}
	ReportEntry(){}
};

struct ComponentReport{
	// relevant data to report
	unordered_map<string, ReportEntry> data;
};

/**
 Implement this interface on a component to offer RuntimeInformation.
 */
class ComponentReportInterface{
	public:
		virtual ComponentReport prepareReport() = 0;
};

}

#endif