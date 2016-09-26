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
#include <unistd.h>
#include <iomanip>

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

 	stringstream buff;
 	buff << opts.output << (long long unsigned) getpid();
  ofile.open(buff.str());  


//	regex = opts.regex; // not implemented?

	DereferencingFacadeOptions& facadeOpts = facade->getOptions<DereferencingFacadeOptions>();
	o = dynamic_cast<monitoring::Ontology*>(facadeOpts.ontology.componentPointer);
	assert(o != nullptr);
  file_limit = opts.file_limit;

  if (0 == opts.interface.compare("POSIX")) {
	  io_iface = IOInterface::POSIX;
  }
  else if (0 == opts.interface.compare("MPI")) {
	  io_iface = IOInterface::MPI;
  }
  else {
    perror("Bad interface");
    exit(1);
  }



	switch (io_iface) 
	{
		case IOInterface::POSIX:
			addActivityHandler("POSIX", "", "open",      & JobStatsPlugin::handleOpen);
			addActivityHandler("POSIX", "", "close",     & JobStatsPlugin::handleClose);
			addActivityHandler("POSIX", "", "creat",     & JobStatsPlugin::handleOpen);

			addActivityHandler("POSIX", "", "read",      & JobStatsPlugin::handleRead);
			addActivityHandler("POSIX", "", "pread",     & JobStatsPlugin::handleRead);
			addActivityHandler("POSIX", "", "readv",     & JobStatsPlugin::handleRead);

			addActivityHandler("POSIX", "", "pwritev",   & JobStatsPlugin::handleWrite);
			addActivityHandler("POSIX", "", "pwrite",     & JobStatsPlugin::handleWrite);
			addActivityHandler("POSIX", "", "write",     & JobStatsPlugin::handleWrite);

			addActivityHandler("POSIX", "", "sync",      & JobStatsPlugin::handleSync);
			addActivityHandler("POSIX", "", "fdatasync", & JobStatsPlugin::handleSync);
			addActivityHandler("POSIX", "", "lseek",     & JobStatsPlugin::handleSeek);

			IGNORE_ERROR(fhID[IOInterface::POSIX]           = o->lookup_attribute_by_name("POSIX", "descriptor/filehandle").aID;)
			IGNORE_ERROR(fname[IOInterface::POSIX]          = o->lookup_attribute_by_name("POSIX", "descriptor/filename").aID;)
			IGNORE_ERROR(bytesReadID[IOInterface::POSIX]    = o->lookup_attribute_by_name("POSIX", "quantity/BytesRead").aID;)
			IGNORE_ERROR(positionID[IOInterface::POSIX]     = o->lookup_attribute_by_name("POSIX", "file/position").aID;)
			IGNORE_ERROR(bytesWrittenID[IOInterface::POSIX] = o->lookup_attribute_by_name("POSIX", "quantity/BytesWritten").aID;)
			break;
		case IOInterface::MPI:
			addActivityHandler("MPI", "Generic", "MPI_File_open",  & JobStatsPlugin::handleOpen);
			addActivityHandler("MPI", "Generic", "MPI_File_read",  & JobStatsPlugin::handleRead);
			addActivityHandler("MPI", "Generic", "MPI_File_write", & JobStatsPlugin::handleWrite);
			addActivityHandler("MPI", "Generic", "MPI_File_close", & JobStatsPlugin::handleClose);

			IGNORE_ERROR(fhID[IOInterface::MPI]           = o->lookup_attribute_by_name("MPI", "descriptor/filehandle").aID;)
			IGNORE_ERROR(fname[IOInterface::MPI]          = o->lookup_attribute_by_name("MPI", "descriptor/filename").aID;)
			IGNORE_ERROR(bytesReadID[IOInterface::MPI]    = o->lookup_attribute_by_name("MPI", "quantity/BytesToRead").aID;)
			IGNORE_ERROR(positionID[IOInterface::MPI]     = o->lookup_attribute_by_name("MPI", "file/position").aID;)
			IGNORE_ERROR(bytesWrittenID[IOInterface::MPI] = o->lookup_attribute_by_name("MPI", "quantity/BytesToWrite").aID;)
			break;
	}

	multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>(&JobStatsPlugin::notify), false);	
}



void JobStatsPlugin::addActivityHandler (const string & interface, const string & impl, const string & a, void (JobStatsPlugin::* handler)(std::shared_ptr<Activity>)) {
//	SystemInformationGlobalIDManager* s = tr->getSystemInformationGlobalIDManager();
	SystemInformationGlobalIDManager* s = facade->get_system_information();
	UniqueInterfaceID uiid;
	try { 
		uiid = s->lookup_interfaceID(interface, impl);
	}
	catch (NotFoundError & e) {
		cout << "Error: interface/implementation not found: " << interface << "/" << impl <<  std::endl;
		exit(1);
	}

	try {
		UniqueComponentActivityID  ucaid = s->lookup_activityID(uiid, a);
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



static void print_bullshit(std::ofstream& ofile, const std::vector<Access>& accesses) {
	size_t rnd_access = 0;
	size_t seq_access = 0;
	size_t next_seq_pos = 0;

	for (const auto access : accesses) {
    switch (access.type) {
      case IOAccessType::WRITE:
        ofile << setw(25) << "write ";
        break;
      case IOAccessType::READ:
        ofile << setw(25) << "read ";
        break;
    }

		ofile 
			<< setw(10) << right << "[time " << setw(10) << access.endTime - access.startTime <<  " ns]" 
			<< setw(10) << right << "[offset " << setw(10) << access.offset << " bytes]" 
			<< setw(10) << right << "[size " << setw(10) << access.size << " bytes]" << std::endl;

		if (access.offset + access.size != next_seq_pos) {
			++rnd_access;
		}
		else {
			++seq_access;
		}
		next_seq_pos = access.offset + access.size;
	}
	ofile << setw(25) << "count_rnd_accesses " << rnd_access << std::endl;
  ofile << setw(25) << "count_seq_accesses " << seq_access << std::endl;
}



void JobStatsPlugin::printFileAccess (const OpenFiles& file) {
  ofile << file.name << std::endl;
  ofile << setw(25) << "open " << setw(20) << right << file.openTime << " ns" << std::endl;
  ofile << setw(25) << "close " << setw(20) << right << file.closeTime << " ns" << std::endl;
	ofile << setw(25) << "duration " << file.closeTime - file.openTime << " ns" << std::endl;
	print_bullshit(ofile, file.accesses);
  size_t bytesRead    = std::accumulate(file.accesses.begin(), file.accesses.end(), (size_t) 0, [](const size_t sum, const Access& access){return (IOAccessType::READ  == access.type) ? sum + access.size : sum;});
  size_t bytesWritten = std::accumulate(file.accesses.begin(), file.accesses.end(), (size_t) 0, [](const size_t sum, const Access& access){return (IOAccessType::WRITE == access.type) ? sum + access.size : sum;});
  ofile << setw(25) << "written " << bytesWritten << " bytes" << std::endl;
  ofile << setw(25) << "read " << bytesRead << " bytes" << std::endl;
	ofile << std::endl;
}



static bool comp(const OpenFiles& f1, const OpenFiles f2) {
  size_t size1 = f1.accesses.size() + f1.syncOperations.size();
  size_t size2 = f2.accesses.size() + f2.syncOperations.size();
  return size1 > size2;
}



void JobStatsPlugin::finalize() {
  std::vector<OpenFiles> tmp;
	for (const auto file : openFiles) {
		tmp.push_back(file.second);
	}
	for (const auto file : unnamedFiles) {
		tmp.push_back(file.second);
	}

  sort(tmp.begin(), tmp.end(), comp);

  size_t counter = 0;
	for (const auto file : tmp) {
		printFileAccess(file);
    if (0 != file_limit && ++counter >= file_limit) {
      break;
    }
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
		uint32_t fh = findUINT32AttributeByID(a, fhID[io_iface]);

		if (unnamedFiles.count(fh) > 0) {
			parent = & unnamedFiles[fh];
		}
		else {
			stringstream s;
			s << "generic_filename_" << ++fh_counter;
			unnamedFiles[fh] = {s.str(), a->time_start_, fh + 10000000};
			parent = & unnamedFiles[fh];
		}
	}
	return parent;
}



void JobStatsPlugin::handleSeek (std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	parent->currentPosition = findUINT64AttributeByID(a, positionID[io_iface]);
}



void JobStatsPlugin::handleSync(std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	parent->syncOperations.push_back({a->time_start_, a->time_stop_});
}



void JobStatsPlugin::handleWrite (std::shared_ptr<Activity> a) {
	uint64_t bytes = findUINT64AttributeByID(a, bytesWrittenID[io_iface]);
	uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);

	OpenFiles* parent = findParentFileByFh(a);
	if (position == INVALID_UINT64) {
		position = parent->currentPosition;
		parent->currentPosition += bytes;
	}
	parent->accesses.push_back(Access{IOAccessType::WRITE, a->time_start_, a->time_stop_, position, bytes});
}



void JobStatsPlugin::handleRead (std::shared_ptr<Activity> a) {
	uint64_t bytes = findUINT64AttributeByID(a, bytesReadID[io_iface]);
	uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);

	OpenFiles* parent = findParentFileByFh(a);
	if (position == INVALID_UINT64) {
		position = parent->currentPosition;
		parent->currentPosition += bytes;
	}
	parent->accesses.push_back(Access{IOAccessType::READ, a->time_start_, a->time_stop_, position, bytes});
}



void JobStatsPlugin::handleOpen (std::shared_ptr<Activity> a) {
  const string name{findStrAttributeByID(a, fname[io_iface])};
	openFiles[a->aid()] = {name, a->time_start_, 0, 0, a->aid_};
}



void JobStatsPlugin::handleClose (std::shared_ptr<Activity> a) {
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
