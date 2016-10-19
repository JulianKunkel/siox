/*
 * =====================================================================================
 *
 *       Filename:  FileAccessInfo.cpp
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
#include <algorithm>

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>

#include "FileAccessInfo.hpp"

using namespace std;
using namespace monitoring;

#define INVALID_UINT64 ((uint64_t) -1)
#define IGNORE_ERROR(x) try{ x } catch(NotFoundError & e){}

FileAccessInfoPluginOptions* FileAccessInfoPlugin::AvailableOptions() {
	return new FileAccessInfoPluginOptions{};
}

void FileAccessInfoPlugin::initPlugin() {
	//vector<string> supportedOpens("MPI_File_open", 	"open", "creat");
	//vector<string> supportedClose("MPI_File_close", "close");
	//vector<string> supportedAccess("read", "write", "pread", "pwrite");
//	o = tr->getOntology();
	
	FileAccessInfoPluginOptions& opts = getOptions<FileAccessInfoPluginOptions>();
	verbosity = opts.verbosity;

	sys_info = facade->get_system_information();

 	stringstream buff;
 	buff << opts.output << (long long unsigned) getpid();
  ofile.open(buff.str());  

	enableSyscallStats = opts.enableSyscallStats;
	enableTrace = opts.enableTrace;


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
			addActivityHandler("POSIX", "", "open",      & FileAccessInfoPlugin::handleOpen);
			addActivityHandler("POSIX", "", "creat",     & FileAccessInfoPlugin::handleOpen);
			addActivityHandler("POSIX", "", "open64",     & FileAccessInfoPlugin::handleOpen);
			addActivityHandler("POSIX", "", "fopen64",     & FileAccessInfoPlugin::handleOpen);
			addActivityHandler("POSIX", "", "fopen",     & FileAccessInfoPlugin::handleOpen);
			addActivityHandler("POSIX", "", "fdopen",     & FileAccessInfoPlugin::handleOpen);

			addActivityHandler("POSIX", "", "close",     & FileAccessInfoPlugin::handleClose);
			addActivityHandler("POSIX", "", "fclose",     & FileAccessInfoPlugin::handleClose);

			addActivityHandler("POSIX", "", "read",      & FileAccessInfoPlugin::handleRead);
			addActivityHandler("POSIX", "", "pread",     & FileAccessInfoPlugin::handleRead);
			addActivityHandler("POSIX", "", "readv",     & FileAccessInfoPlugin::handleRead);
			addActivityHandler("POSIX", "", "fgets",     & FileAccessInfoPlugin::handleRead);
			addActivityHandler("POSIX", "", "gets",      & FileAccessInfoPlugin::handleRead);
			addActivityHandler("POSIX", "", "fread",     & FileAccessInfoPlugin::handleRead);

			addActivityHandler("POSIX", "", "pwritev",   & FileAccessInfoPlugin::handleWrite);
			addActivityHandler("POSIX", "", "pwrite",    & FileAccessInfoPlugin::handleWrite);
			addActivityHandler("POSIX", "", "write",     & FileAccessInfoPlugin::handleWrite);
			addActivityHandler("POSIX", "", "fwrite",    & FileAccessInfoPlugin::handleWrite);
			addActivityHandler("POSIX", "", "fputs",     & FileAccessInfoPlugin::handleWrite);
			addActivityHandler("POSIX", "", "puts",      & FileAccessInfoPlugin::handleWrite);
			addActivityHandler("POSIX", "", "fputc",      & FileAccessInfoPlugin::handleWrite);

			addActivityHandler("POSIX", "", "sync",      & FileAccessInfoPlugin::handleSync);
			addActivityHandler("POSIX", "", "fdatasync", & FileAccessInfoPlugin::handleSync);

			addActivityHandler("POSIX", "", "lseek",     & FileAccessInfoPlugin::handleSeek);

			IGNORE_ERROR(fhID[IOInterface::POSIX]           = o->lookup_attribute_by_name("POSIX", "descriptor/filehandle").aID;)
			IGNORE_ERROR(fname[IOInterface::POSIX]          = o->lookup_attribute_by_name("POSIX", "descriptor/filename").aID;)
			IGNORE_ERROR(bytesReadID[IOInterface::POSIX]    = o->lookup_attribute_by_name("POSIX", "quantity/BytesRead").aID;)
			IGNORE_ERROR(bytesToReadID[IOInterface::POSIX]    = o->lookup_attribute_by_name("POSIX", "quantity/BytesToRead").aID;)
			IGNORE_ERROR(positionID[IOInterface::POSIX]     = o->lookup_attribute_by_name("POSIX", "file/position").aID;)
			IGNORE_ERROR(bytesWrittenID[IOInterface::POSIX] = o->lookup_attribute_by_name("POSIX", "quantity/BytesWritten").aID;)
			IGNORE_ERROR(bytesToWriteID[IOInterface::POSIX] = o->lookup_attribute_by_name("POSIX", "quantity/BytesToWrite").aID;)
			break;
		case IOInterface::MPI:
			addActivityHandler("MPI", "Generic", "MPI_File_open",  & FileAccessInfoPlugin::handleOpen);
			addActivityHandler("MPI", "Generic", "MPI_File_read",  & FileAccessInfoPlugin::handleRead);
			addActivityHandler("MPI", "Generic", "MPI_File_write", & FileAccessInfoPlugin::handleWrite);
			addActivityHandler("MPI", "Generic", "MPI_File_close", & FileAccessInfoPlugin::handleClose);

			IGNORE_ERROR(fhID[IOInterface::MPI]           = o->lookup_attribute_by_name("MPI", "descriptor/filehandle").aID;)
			IGNORE_ERROR(fname[IOInterface::MPI]          = o->lookup_attribute_by_name("MPI", "descriptor/filename").aID;)
			IGNORE_ERROR(bytesReadID[IOInterface::MPI]  = o->lookup_attribute_by_name("MPI", "quantity/BytesRead").aID;)
			IGNORE_ERROR(bytesToReadID[IOInterface::MPI]  = o->lookup_attribute_by_name("MPI", "quantity/BytesToRead").aID;)
			IGNORE_ERROR(positionID[IOInterface::MPI]     = o->lookup_attribute_by_name("MPI", "file/position").aID;)
			IGNORE_ERROR(bytesWrittenID[IOInterface::MPI] = o->lookup_attribute_by_name("MPI", "quantity/BytesWritten").aID;)
			IGNORE_ERROR(bytesToWriteID[IOInterface::MPI] = o->lookup_attribute_by_name("MPI", "quantity/BytesToWrite").aID;)
			break;
	}

	multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>(&FileAccessInfoPlugin::notify), false);	
}



void FileAccessInfoPlugin::addActivityHandler (const string & interface, const string & impl, const string & a, void (FileAccessInfoPlugin::* handler)(std::shared_ptr<Activity>)) {
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



void FileAccessInfoPlugin::notify (const std::shared_ptr<Activity>& a, int lost) {
	auto both = activityHandlers.find(a->ucaid_);

	accessCounter[a->ucaid()]++;

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



//static void print_bullshit(std::ofstream& ofile, const std::vector<Access>& accesses) {
//}



void FileAccessInfoPlugin::printFileAccess (const OpenFiles& file) {
  ofile << setw(25) << "filename " << file.name << std::endl;
  ofile << setw(25) << "open duration " << file.openDuration << " ns" << ((0 == file.openDuration) ? " (file was already open?)" : "") << std::endl;
  ofile << setw(25) << "close duration " << file.closeDuration << " ns" << ((0 == file.closeDuration) ? " (file was not closed?)" : "") << std::endl;
//	ofile << setw(25) << "open-close duration " << file.closeTime - file.openTime << " ns" << std::endl;
  size_t bytesRead    = std::accumulate(file.accesses.begin(), file.accesses.end(), (size_t) 0, [](const size_t sum, const Access& access){return (IOAccessType::READ  == access.type) ? sum + access.size : sum;});
  size_t bytesWritten = std::accumulate(file.accesses.begin(), file.accesses.end(), (size_t) 0, [](const size_t sum, const Access& access){return (IOAccessType::WRITE == access.type) ? sum + access.size : sum;});
  ofile << setw(25) << "written data " << bytesWritten << " bytes" << std::endl;
  ofile << setw(25) << "read data " << bytesRead << " bytes" << std::endl;

	{
		size_t rnd_access = 0;
		size_t seq_access = 0;
		size_t next_seq_pos = 0;
		size_t read_time = 0;
		size_t write_time = 0;

		std::stringstream ss;

		for (const auto access : file.accesses) {
			switch (access.type) {
				case IOAccessType::WRITE:
					ss << setw(25) << "write ";
					write_time += access.endTime - access.startTime;
					break;
				case IOAccessType::READ:
					ss << setw(25) << "read ";
					read_time += access.endTime - access.startTime;
					break;
				case IOAccessType::SYNC:
					break;
			}

			ss
				<< setw(10) << right << "[time " << setw(10) << access.endTime - access.startTime <<  " ns]" 
				<< setw(10) << right << "[offset " << setw(10) << access.offset << " bytes]" 
				<< setw(10) << right << "[size " << setw(10) << access.size << " bytes]" << std::endl;

			if (access.offset != next_seq_pos) {
				++rnd_access;
			}
			else {
				++seq_access;
			}
			next_seq_pos = access.offset + access.size;
		}
		ofile << setw(25) << "write duration " << write_time << " ns" << std::endl;
		ofile << setw(25) << "read duration " << read_time << " ns" << std::endl;
		ofile << setw(25) << "count_rnd_accesses " << rnd_access << std::endl;
		ofile << setw(25) << "count_seq_accesses " << seq_access << std::endl;
		if (0 != enableTrace) {
			ofile << ss.str();
		}
	}

	ofile << std::endl;
}



static bool comp(const OpenFiles& f1, const OpenFiles f2) {
  const size_t size1 = std::accumulate(f1.accesses.begin(), f1.accesses.end(), (size_t) 0, [](const size_t sum, const Access& access){return sum + access.endTime - access.startTime;});
  const size_t size2 = std::accumulate(f2.accesses.begin(), f2.accesses.end(), (size_t) 0, [](const size_t sum, const Access& access){return sum + access.endTime - access.startTime;});
  return size1 > size2;
}



void FileAccessInfoPlugin::finalize() {
	const string sep(100, '*');
	if (0 != enableSyscallStats) {
		ofile << sep << std::endl;
		for (const auto ac : accessCounter) {
			ofile << "syscall: " <<  setw(15) << sys_info->lookup_activity_name(ac.first) << " " << ac.second;
			if (activityHandlers.find(ac.first) == activityHandlers.end()) {
				ofile << " (not handled)";
			}
			ofile << std::endl;
		}
		ofile << sep << std::endl;
	}
	
	std::vector<OpenFiles> tmp_files;
	for (const auto file : openFiles) {
		tmp_files.push_back(file.second);
	}
	for (const auto file : unnamedFiles) {
		tmp_files.push_back(file.second);
	}

  sort(tmp_files.begin(), tmp_files.end(), comp);

  size_t counter = 0;
	for (const auto file : tmp_files) {
		printFileAccess(file);
		// print all file stats, if file_limit == 0
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



OpenFiles* FileAccessInfoPlugin::findParentFile (const std::shared_ptr<Activity> a) {
  for (const auto aid : a->parentArray()) {
    auto openFile = openFiles.find(aid);
    if (openFiles.end() != openFile) {
      return &openFile->second;
    }
  }
	return nullptr;
}



OpenFiles* FileAccessInfoPlugin::findParentFileByFh (const std::shared_ptr<Activity> a) {
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



void FileAccessInfoPlugin::handleSeek (std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	parent->currentPosition = findUINT64AttributeByID(a, positionID[io_iface]);
}



void FileAccessInfoPlugin::handleSync(std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	parent->syncOperations.push_back({a->time_start_, a->time_stop_});
}



void FileAccessInfoPlugin::handleWrite (std::shared_ptr<Activity> a) {
	uint64_t bytesWritten = findUINT64AttributeByID(a, bytesWrittenID[io_iface]);
	uint64_t bytesToWrite = findUINT64AttributeByID(a, bytesToWriteID[io_iface]);
	uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);

	OpenFiles* parent = findParentFileByFh(a);
	if (position == INVALID_UINT64) {
		position = parent->currentPosition;
		parent->currentPosition += bytesWritten;
	}
  if (0 == a->errorValue() && bytesWritten == bytesToWrite && INVALID_UINT64 != bytesWritten) {
    parent->accesses.push_back(Access{IOAccessType::WRITE, a->time_start_, a->time_stop_, position, bytesWritten});
  }
}



void FileAccessInfoPlugin::handleRead (std::shared_ptr<Activity> a) {
	uint64_t bytesRead = findUINT64AttributeByID(a, bytesReadID[io_iface]);
	uint64_t bytesToRead = findUINT64AttributeByID(a, bytesToReadID[io_iface]);
	uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);
//  for (const auto attribute : a->attributeArray()) {
//	  OntologyAttributeFull oa = facade->lookup_attribute_by_ID( attribute.id );
//    std::cout << oa.name << " " << attribute.value << std::endl;
//  }
//  std::cout << "bytesRead: " << bytesRead << " " << "bytesToRead: " << bytesToRead << std::endl;


	OpenFiles* parent = findParentFileByFh(a);
	if (position == INVALID_UINT64) {
		position = parent->currentPosition;
		parent->currentPosition += bytesRead;
	}

  if (0 == a->errorValue() && bytesRead == bytesToRead && INVALID_UINT64 != bytesRead) {
	  parent->accesses.push_back(Access{IOAccessType::READ, a->time_start_, a->time_stop_, position, bytesRead});
  }
}



void FileAccessInfoPlugin::handleOpen (std::shared_ptr<Activity> a) {
  const string name{findStrAttributeByID(a, fname[io_iface])};
	openFiles[a->aid()] = {name, a->time_start_, 0, a->time_stop_ - a->time_start_, 0, 0, a->aid_};
}



void FileAccessInfoPlugin::handleClose (std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	assert(nullptr != parent);
	parent->closeTime = a->time_stop_;
  parent->closeDuration = a->time_stop_ - a->time_start_;
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new FileAccessInfoPlugin();
	}
}
