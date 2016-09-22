/*
 * =====================================================================================
 *
 *       Filename:  JobStats.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2016 03:02:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <regex>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cassert>

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>

#include "JobStats.hpp"

using namespace std;
using namespace monitoring;

#define INVALID_UINT64 ((uint64_t) -1)
#define IGNORE_ERROR(x) try{ x } catch(NotFoundError & e){}

JobStatsPluginOptions* JobStatsPlugin::AvailableOptions() {
	return new JobStatsPluginOptions{};
}

void JobStatsPlugin::initPlugin() {
	//vector<string> supportedOpens("MPI_File_open", 	"open", "creat");
	//vector<string> supportedClose("MPI_File_close", "close");
	//vector<string> supportedAccess("read", "write", "pread", "pwrite");
//	o = tr->getOntology();
	
	JobStatsPluginOptions& opts = getOptions<JobStatsPluginOptions>();
	verbosity = opts.verbosity;
  ofile.open(opts.output);  


//	regex = opts.regex; // not implemented?

	DereferencingFacadeOptions& facadeOpts = facade->getOptions<DereferencingFacadeOptions>();
	o = dynamic_cast<monitoring::Ontology*>(facadeOpts.ontology.componentPointer);
	assert(o != nullptr);

	addActivityHandler("POSIX", "", "open",      & JobStatsPlugin::handlePOSIXOpen);
	addActivityHandler("POSIX", "", "creat",     & JobStatsPlugin::handlePOSIXOpen);
	addActivityHandler("POSIX", "", "write",     & JobStatsPlugin::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "read",      & JobStatsPlugin::handlePOSIXRead);
	addActivityHandler("POSIX", "", "pwrite",    & JobStatsPlugin::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "pread",     & JobStatsPlugin::handlePOSIXRead);
	addActivityHandler("POSIX", "", "write",     & JobStatsPlugin::handlePOSIXWrite);
	addActivityHandler("POSIX", "", "readv",     & JobStatsPlugin::handlePOSIXRead);
	addActivityHandler("POSIX", "", "close",     & JobStatsPlugin::handlePOSIXClose);
	addActivityHandler("POSIX", "", "sync",      & JobStatsPlugin::handlePOSIXSync);
	addActivityHandler("POSIX", "", "fdatasync", & JobStatsPlugin::handlePOSIXSync);
	addActivityHandler("POSIX", "", "lseek",     & JobStatsPlugin::handlePOSIXSeek);

	IGNORE_ERROR(fhID           = o->lookup_attribute_by_name( "POSIX", "descriptor/filehandle" ).aID;)
	IGNORE_ERROR(fname          = o->lookup_attribute_by_name( "POSIX", "descriptor/filename" ).aID;)
	IGNORE_ERROR(bytesReadID    = o->lookup_attribute_by_name( "POSIX", "quantity/BytesRead" ).aID;)
	IGNORE_ERROR(positionID     = o->lookup_attribute_by_name( "POSIX", "file/position" ).aID;)
	IGNORE_ERROR(bytesWrittenID = o->lookup_attribute_by_name( "POSIX", "quantity/BytesWritten" ).aID;)

	multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>(&JobStatsPlugin::notify), false);	
}



void JobStatsPlugin::addActivityHandler (const string & interface, const string & impl, const string & a, void (JobStatsPlugin::* handler)(std::shared_ptr<Activity>)) {
//	SystemInformationGlobalIDManager * s = tr->getSystemInformationGlobalIDManager();
	SystemInformationGlobalIDManager * s = facade->get_system_information();
	UniqueInterfaceID uiid = s->lookup_interfaceID( interface, impl );

	try {
		UniqueComponentActivityID  ucaid = s->lookup_activityID( uiid, a );
		activityHandlers[ucaid] = handler;
	}
	catch (NotFoundError & e) {
		// we are not installing the handler if the type is unknown
	}
}



void JobStatsPlugin::notify (const std::shared_ptr<Activity>& a, int lost) {
	auto both = activityHandlers.find(a->ucaid_);

	if (both != activityHandlers.end()) {
		auto fkt = both->second;

		if (verbosity > 3) {
//			std::cout << __PRETTY_FUNCTION__ << std::endl;
			//			tr->printActivity(a);
		}

		(this->*fkt)(a);
	}
}



static double convertTime(Timestamp time) {
	constexpr double factor = 0.001 * 0.001 * 0.001;
	return time * factor;
}



static void print_bullshit(std::ofstream& ofile, const std::vector<Access>& accesses, const std::string& access_name) {
	size_t rnd_access = 0;
	size_t seq_access = 0;
	size_t sum = 0;
	size_t next_seq_pos = 0;

	for (const auto access : accesses) {
		ofile << access_name  << " " <<  access.startTime << " " << access.endTime << " " << access.offset << " " << access.size << std::endl;
		sum += access.size;

		if (access.offset + access.size != next_seq_pos) {
			++rnd_access;
		}
		else {
			++seq_access;
		}
		next_seq_pos = access.offset + access.size;
	}
	ofile << "sum: " << sum << " bytes" << std::endl;
	ofile << "rnd_access: " << rnd_access << " " << "seq_access: " <<  seq_access <<  std::endl;
}



void JobStatsPlugin::printFileAccess (const OpenFiles& file) {
  ofile << file.name << std::endl;
  ofile << "open: " << file.openTime << std::endl;
  ofile << "close: " << file.closeTime << std::endl;
	print_bullshit(ofile, file.readAccesses, "read");
	print_bullshit(ofile, file.writeAccesses, "write");
	ofile << std::endl;
}



void JobStatsPlugin::finalize() {
	mkdir("plot", 0777);
	for (const auto file : openFiles) {
		printFileAccess(file.second);
	}
	for (const auto file : unnamedFiles) {
		printFileAccess(file.second);
	}
}



static const uint32_t findUINT32AttributeByID (const std::shared_ptr<Activity> a, OntologyAttributeID oaid) {
 	const vector<Attribute>& attributes = a->attributeArray();
	for (const auto attribute : attributes) {
		if (attribute.id == oaid)
			return attribute.value.uint32();
	}
	return 0;
}



static const uint64_t findUINT64AttributeByID (const std::shared_ptr<Activity> a, OntologyAttributeID oaid) {
 	const vector<Attribute> & attributes = a->attributeArray();
	for (const auto attribute : attributes) {
		if (attribute.id == oaid)
			return attribute.value.uint64();
	}
	return INVALID_UINT64;
}



static const string findStrAttributeByID (const std::shared_ptr<Activity> a, OntologyAttributeID oaid) {
 	const vector<Attribute>& attributes = a->attributeArray();
	for (const auto attribute : attributes) {
		if (attribute.id == oaid)
			return string{attribute.value.str()};
	}
	return "unknown";
}



OpenFiles* JobStatsPlugin::findParentFile (const std::shared_ptr<Activity> a) {
  for (const auto aid : a->parentArray()) {
    auto openFile = openFiles.find(aid);
    if (openFiles.end() != openFile) {
      return &openFile->second;
    }
  }
	return nullptr;
}



OpenFiles* JobStatsPlugin::findParentFileByFh (const std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFile(a);
	if (nullptr == parent) {
		// add a dummy for the file handle since we do not know the filename
		uint32_t fh = findUINT32AttributeByID(a, fhID);

		if (unnamedFiles.count(fh) > 0) {
			parent = & unnamedFiles[fh];
		}
		else {
			stringstream s;
			s << "generic_file_" << ++fh_counter;
			unnamedFiles[fh] = {s.str(), a->time_start_, fh + 10000000};
			parent = & unnamedFiles[fh];
		}
	}
	return parent;
}



void JobStatsPlugin::handlePOSIXSeek (std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	parent->currentPosition = findUINT64AttributeByID(a, positionID);
}



void JobStatsPlugin::handlePOSIXSync (std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	parent->syncOperations.push_back({a->time_start_, a->time_stop_});
}



void JobStatsPlugin::handlePOSIXWrite (std::shared_ptr<Activity> a) {
	uint64_t bytes = findUINT64AttributeByID(a, bytesWrittenID);
	uint64_t position = findUINT64AttributeByID(a, positionID);

	OpenFiles* parent = findParentFileByFh(a);
	if (position == INVALID_UINT64) {
		position = parent->currentPosition;
		parent->currentPosition += bytes;
	}
	parent->writeAccesses.push_back(Access{a->time_start_, a->time_stop_, position, bytes});
}



void JobStatsPlugin::handlePOSIXRead (std::shared_ptr<Activity> a) {
	uint64_t bytes = findUINT64AttributeByID(a, bytesReadID);
	uint64_t position = findUINT64AttributeByID(a, positionID);

	OpenFiles* parent = findParentFileByFh(a);
	if (position == INVALID_UINT64) {
		position = parent->currentPosition;
		parent->currentPosition += bytes;
	}
	parent->readAccesses.push_back(Access{a->time_start_, a->time_stop_, position, bytes});
}



void JobStatsPlugin::handlePOSIXOpen (std::shared_ptr<Activity> a) {
  const string name{findStrAttributeByID(a, fname)};
	openFiles[a->aid()] = {name, a->time_start_, 0, 0, a->aid_};
}



void JobStatsPlugin::handlePOSIXClose (std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	assert(nullptr != parent);
	parent->closeTime = a->time_stop_;
}



extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new JobStatsPlugin();
	}
}
