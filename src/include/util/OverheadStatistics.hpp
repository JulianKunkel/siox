#ifndef CORE_OVERHEAD_STATISTICS_HPP
#define CORE_OVERHEAD_STATISTICS_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <mutex> 

#include <monitoring/datatypes/GenericTypes.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <core/component/Component.hpp>

/*
 This generic component can be used to capture and report the overhead of SIOX.
 */

namespace util{

struct OverheadEntry{	
	// protect this entry
	std::mutex m;

	// Overhead time in ns.
	uint64_t time = 0;
	uint64_t occurence = 0;
};

class OverheadStatistics{
public:
	void appendReport(std::unordered_map<core::GroupEntry* , core::ReportEntry> & map);

	Timestamp startMeasurement();
	void stopMeasurement(const std::string & what, const Timestamp & start);
	void stopMeasurement(OverheadEntry & entry, const Timestamp & start);

	OverheadEntry & getOverheadFor(const std::string & what);
private:
	// protect the map
	std::mutex m;

	std::unordered_map< std::string, OverheadEntry> entries;
};

class OverheadStatisticsDummy : public core::Component, public core::ComponentReportInterface{
public:
    virtual core::ComponentOptions* AvailableOptions(){
            return nullptr;
    }

    virtual void init(){
    };

    OverheadStatisticsDummy(OverheadStatistics & stats) : stats(stats) {}

    core::ComponentReport prepareReport();
private:
	 OverheadStatistics & stats;
};


}

#endif
