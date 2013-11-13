#include <util/OverheadStatistics.hpp>
#include <util/time.h>

using namespace std;
using namespace util;
using namespace core;



Timestamp OverheadStatistics::startMeasurement(){
	return siox_gettime();
}

void OverheadStatistics::stopMeasurement(const std::string & what, const Timestamp & start){
	stopMeasurement(getOverheadFor(what), start);
}

void OverheadStatistics::stopMeasurement(OverheadEntry & entry, const Timestamp & start){
	Timestamp delta = siox_gettime() - start;
	entry.occurence++;
	entry.time += delta;

	all.time += delta;
	all.occurence++;
}

OverheadEntry & OverheadStatistics::getOverheadFor(const std::string & what){
	assert(what.size() > 2);
	
	auto itr = entries.find(what);
	if ( itr == entries.end() ){
		entries[what] = OverheadEntry();
		return entries[what];
	}
	return itr->second;
}


void OverheadStatistics::appendReport(unordered_map<string, ReportEntry> & map){
	for( auto itr = entries.begin(); itr != entries.end() ; itr++ ){
		const string & name = itr->first;
		const OverheadEntry & entry = itr->second;
		map["COUNT_" + name] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, entry.occurence };
		map["TIME_" + name] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, entry.time / 1000000000.0};
	}

	// add sum with a high priority:
	map["COUNT_ALL_EVENTS"] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, all.occurence, 200 };
	map["TIME_ALL_EVENTS"] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, all.time / 1000000000.0, 200 };
}

core::ComponentReport OverheadStatisticsDummy::prepareReport(){
	ComponentReport rep;
	stats.appendReport( rep.data );
	return rep;
}