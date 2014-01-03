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

void OverheadStatistics::stopMeasurement(OverheadEntry * entry, const Timestamp & start){
	Timestamp delta = siox_gettime() - start;

	entry->occurrence_++;
	entry->time_ += delta;
}

OverheadEntry * OverheadStatistics::getOverheadFor(const std::string & what){
	assert(what.size() > 2);

	unique_lock<mutex> lock( m );
	
	auto itr = entries.find(what);
	if ( itr == entries.end() ){
		OverheadEntry * ptr = new OverheadEntry();
		entries[what] = ptr;
		return ptr;
	}
	return itr->second;
}

void OverheadStatistics::appendReport(unordered_map<core::GroupEntry* , ReportEntry> & map){
	uint64_t all_occurrences = 0;
	uint64_t all_time = 0;

	for( auto itr = entries.begin(); itr != entries.end() ; itr++ ){
		const string & name = itr->first;
		const OverheadEntry & entry = *itr->second;

		auto group = new GroupEntry(name);
		auto count = new GroupEntry("count", group);
		auto time = new GroupEntry("time", group);

		map[count] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, entry.occurrence() };
		map[time] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, entry.time() / 1000000000.0};

		all_occurrences += entry.occurrence();
		all_time += entry.time();
	}

	// add sum with a high priority:
	{
	auto group = new GroupEntry("all");
	auto count = new GroupEntry("count", group);
	auto time = new GroupEntry("time", group);

	map[count] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, all_occurrences, 200 };
	map[time] = {ReportEntry::Type::SIOX_INTERNAL_PERFORMANCE, all_time / 1000000000.0, 200 };
	}
}

core::ComponentReport OverheadStatisticsDummy::prepareReport(){
	ComponentReport rep;
	stats.appendReport( rep.data );
	return rep;
}