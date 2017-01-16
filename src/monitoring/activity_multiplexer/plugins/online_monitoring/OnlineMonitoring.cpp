/*
 * =====================================================================================
 *
 *       Filename:  OnlineMonitoring.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/20/2016 03:02:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eugen Betke (betke@dkrz.de), Julian Kunkel (julian.kunkel@googlemail.com)
 *   Organization:  Deutsches Klimarechenzentrum
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
#include <cstdlib>
#include <chrono>
#include <typeinfo>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <numeric>

#include <knowledge/activity_plugin/DereferencingFacadeOptions.hpp>

#include "OnlineMonitoring.hpp"

using namespace std;
using namespace monitoring;

#define INVALID_UINT64 ((uint64_t) -1)
#define IGNORE_ERROR(x) try{ x } catch(NotFoundError & e){}


OnlineMonitoringPluginOptions* OnlineMonitoringPlugin::AvailableOptions() {
	return new OnlineMonitoringPluginOptions{};
}

void OnlineMonitoringPlugin::initPlugin() {
	//vector<string> supportedOpens("MPI_File_open", 	"open", "creat");
	//vector<string> supportedClose("MPI_File_close", "close");
	//vector<string> supportedAccess("read", "write", "pread", "pwrite");

//	o = tr->getOntology();
	
	OnlineMonitoringPluginOptions& opts = getOptions<OnlineMonitoringPluginOptions>();
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

//	assert(nullptr != ontology);
//	assert(nullptr != system_information_manager);
//	assert(nullptr != association_mapper);


	assert(o != nullptr);

  file_limit = opts.file_limit;
	m_tsdb_enabled = opts.tsdb_enabled;
	m_tsdb_username = opts.tsdb_username;
	m_tsdb_password = opts.tsdb_password;
	m_tsdb_host = opts.tsdb_host;
	m_tsdb_port = opts.tsdb_port;

	m_elastic_enabled = opts.elastic_enabled;
	m_elastic_username = opts.elastic_username;
	m_elastic_password = opts.elastic_password;
	m_elastic_host = opts.elastic_host;
	m_elastic_port = opts.elastic_port;


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

  std::unordered_map<IOAccessType, std::vector<std::string>, EnumClassHash> posixOpNames{};

  posixOpNames[IOAccessType::OPEN] = {"open", "creat", "open64", "fopen64", "fopen", "fdopen"};
  posixOpNames[IOAccessType::CLOSE] = {"close", "fclose"};
  posixOpNames[IOAccessType::READ] = {"read", "pread", "readv", "fgets", "gets", "fread"};
  posixOpNames[IOAccessType::WRITE] = {"pwritev", "pwrite", "write", "fwrite", "fputs", "puts", "fputc"};
  posixOpNames[IOAccessType::SYNC] = {"sync", "fdatasync"};
  posixOpNames[IOAccessType::SEEK] = {"lseek"};


	switch (io_iface) 
	{
    case IOInterface::POSIX:
      for (const auto& opName : posixOpNames[IOAccessType::OPEN]) {addActivityHandler("POSIX", "", opName, &OnlineMonitoringPlugin::handleOpen);}
      for (const auto& opName : posixOpNames[IOAccessType::CLOSE]) {addActivityHandler("POSIX", "", opName, &OnlineMonitoringPlugin::handleClose);}
      for (const auto& opName : posixOpNames[IOAccessType::READ]) {addActivityHandler("POSIX", "", opName, &OnlineMonitoringPlugin::handleRead);}
      for (const auto& opName : posixOpNames[IOAccessType::WRITE]) {addActivityHandler("POSIX", "", opName, &OnlineMonitoringPlugin::handleWrite);}
      for (const auto& opName : posixOpNames[IOAccessType::SYNC]) {addActivityHandler("POSIX", "", opName, &OnlineMonitoringPlugin::handleSync);}
      for (const auto& opName : posixOpNames[IOAccessType::SEEK]) {addActivityHandler("POSIX", "", opName, &OnlineMonitoringPlugin::handleSeek);}

      IGNORE_ERROR(fhID[IOInterface::POSIX]           = o->lookup_attribute_by_name("POSIX", "descriptor/filehandle").aID;)
      IGNORE_ERROR(fname[IOInterface::POSIX]          = o->lookup_attribute_by_name("POSIX", "descriptor/filename").aID;)
      IGNORE_ERROR(bytesReadID[IOInterface::POSIX]    = o->lookup_attribute_by_name("POSIX", "quantity/BytesRead").aID;)
      IGNORE_ERROR(bytesToReadID[IOInterface::POSIX]  = o->lookup_attribute_by_name("POSIX", "quantity/BytesToRead").aID;)
      IGNORE_ERROR(positionID[IOInterface::POSIX]     = o->lookup_attribute_by_name("POSIX", "file/position").aID;)
      IGNORE_ERROR(bytesWrittenID[IOInterface::POSIX] = o->lookup_attribute_by_name("POSIX", "quantity/BytesWritten").aID;)
      IGNORE_ERROR(bytesToWriteID[IOInterface::POSIX] = o->lookup_attribute_by_name("POSIX", "quantity/BytesToWrite").aID;)
      break;
    case IOInterface::MPI:
      addActivityHandler("MPI", "Generic", "MPI_File_open",  & OnlineMonitoringPlugin::handleOpen);
      addActivityHandler("MPI", "Generic", "MPI_File_read",  & OnlineMonitoringPlugin::handleRead);
      addActivityHandler("MPI", "Generic", "MPI_File_write", & OnlineMonitoringPlugin::handleWrite);
      addActivityHandler("MPI", "Generic", "MPI_File_close", & OnlineMonitoringPlugin::handleClose);

      IGNORE_ERROR(fhID[IOInterface::MPI]           = o->lookup_attribute_by_name("MPI", "descriptor/filehandle").aID;)
      IGNORE_ERROR(fname[IOInterface::MPI]          = o->lookup_attribute_by_name("MPI", "descriptor/filename").aID;)
      IGNORE_ERROR(bytesReadID[IOInterface::MPI]    = o->lookup_attribute_by_name("MPI", "quantity/BytesRead").aID;)
      IGNORE_ERROR(bytesToReadID[IOInterface::MPI]  = o->lookup_attribute_by_name("MPI", "quantity/BytesToRead").aID;)
      IGNORE_ERROR(positionID[IOInterface::MPI]     = o->lookup_attribute_by_name("MPI", "file/position").aID;)
      IGNORE_ERROR(bytesWrittenID[IOInterface::MPI] = o->lookup_attribute_by_name("MPI", "quantity/BytesWritten").aID;)
      IGNORE_ERROR(bytesToWriteID[IOInterface::MPI] = o->lookup_attribute_by_name("MPI", "quantity/BytesToWrite").aID;)
      break;
  }

	multiplexer->registerCatchall(this, static_cast<ActivityMultiplexer::Callback>(&OnlineMonitoringPlugin::notify), false);	


	/* Online Monitoring with  OpenTSDB */
	if (m_tsdb_enabled || m_elastic_enabled) {
		const char* c_host = (nullptr == getenv("HOSTNAME")) ? "fakehost" : getenv("HOSTNAME");
		const char* c_username = (nullptr == getenv("SLURM_JOB_USER")) ? "fakeuser" : getenv("SLURM_JOB_USER");
		const char* c_jobid = (nullptr == getenv("SLURM_JOBID")) ? "0" : getenv("SLURM_JOBID");
		const char* c_procid = (nullptr == getenv("SLURM_PROCID"))  ? "0" : getenv("SLURM_PROCID");

		assert(nullptr != c_host);
		assert(nullptr != c_procid);
		assert(nullptr != c_jobid);
		assert(nullptr != c_username);

//		int nodeid = atoi(c_nodeid);
//		int procid = atoi(c_procid);
//		int localid = atoi(c_localid);

//		std::stringstream ss;
		m_metric_host = c_host;
		m_metric_username = c_username;
		m_metric_jobid = c_jobid;
		m_metric_procid = c_procid;

		m_tsdb_client.init(m_tsdb_host, m_tsdb_port, m_tsdb_username, m_tsdb_password);
		m_elastic_client.init(m_elastic_host, m_elastic_port, m_elastic_username, m_elastic_password);
		m_thread = std::thread(&OnlineMonitoringPlugin::sendToDB, this);
	}
	/* END: Online Monitoring with  OpenTSDB */
}



void OnlineMonitoringPlugin::addActivityHandler (const string & interface, const string & impl, const string & a, void (OnlineMonitoringPlugin::* handler)(std::shared_ptr<Activity>)) {
//	SystemInformationGlobalIDManager* s = tr->getSystemInformationGlobalIDManager();
	SystemInformationGlobalIDManager* s = facade->get_system_information();
	UniqueInterfaceID uiid;
	try { 
		uiid = s->lookup_interfaceID(interface, impl);
	}
	catch (NotFoundError & e) {
		cerr << "Error: interface/implementation not found: " << interface << "/" << impl <<  std::endl;
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



void OnlineMonitoringPlugin::notify (const std::shared_ptr<Activity>& a, int lost) {
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




static std::string toStr(const IOAccessType type) {
  switch (type) {
    case IOAccessType::WRITE:
      return "write";
    case IOAccessType::READ:
      return "read";
    case IOAccessType::SYNC:
      return "sync";
    case IOAccessType::SEEK:
      return "seek";
    case IOAccessType::OPEN:
      return "open";
    case IOAccessType::CLOSE:
      return "close";
  }
  assert(false);
  return "";
}



void OnlineMonitoringPlugin::aggregate(const IOAccessType access, const Timestamp start, const Timestamp stop, const uint64_t position, const uint64_t bytes, const OpenFiles& file) {
  const string& fn = file.name;
  m_mutex[fn][access].lock();
  m_agg[fn][access].bytes += bytes;
  m_agg[fn][access].count += 1;
  m_agg[fn][access].time += (stop - start);
  m_mutex[fn][access].unlock();
}




void OnlineMonitoringPlugin::sendToDB() {
  using HRC = std::chrono::high_resolution_clock;
  using namespace std::chrono;
  constexpr nanoseconds sleep_duration{seconds{1}}; 
  const HRC::time_point start{HRC::now()};
  unsigned int count{0};

  while(true != m_thread_stop) {
    ++count;
    for (const auto& fnmap : m_agg) {
      const auto& fn = fnmap.first;
      for (const auto& accessmap : fnmap.second) {

        const auto& access = accessmap.first;
  			const HRC::time_point timestamp{HRC::now()};
	
				// Lock -> Fast Copy -> Unlock
        m_mutex[fn][access].lock();
        const MetricAggregation agg = m_agg[fn][access];
        m_agg[fn][access].reset();
        m_mutex[fn][access].unlock();

				std::shared_ptr<Client::Datapoint> point = std::make_shared<Client::Datapoint>();
				point->m_host      = m_metric_host;
				point->m_username  = m_metric_username;
				point->m_jobid     = stol(m_metric_jobid);
				point->m_procid    = stol(m_metric_procid);
				point->m_timestamp = timestamp;
				point->m_access    = toStr(access);
				point->m_filename  = fn;
				point->m_duration  = agg.time;
				point->m_bytes     = agg.bytes;
				point->m_calls     = agg.count;

				// TODO: run, if client enabled
				if (m_tsdb_enabled) {
					m_tsdb_client.enqueue(point);
					m_tsdb_client.send();
				}
				if (m_elastic_enabled) {
					m_elastic_client.enqueue(point);
					m_elastic_client.send();
				}
      }
    }
    std::this_thread::sleep_until(start + count * sleep_duration);
  }
}



void OnlineMonitoringPlugin::printFileAccess (const OpenFiles& file) {
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
					ss << setw(25) << toStr(access.type) << " ";
					write_time += access.endTime - access.startTime;
					break;
				case IOAccessType::READ:
					ss << setw(25) << toStr(access.type) << " ";
					read_time += access.endTime - access.startTime;
					break;
				case IOAccessType::SYNC:
				case IOAccessType::SEEK:
				case IOAccessType::OPEN:
				case IOAccessType::CLOSE:
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



void OnlineMonitoringPlugin::finalize() {
  m_thread_stop = true;
	if (m_tsdb_enabled || m_elastic_enabled) {
		m_thread.join();
	}

  /* SUMMARY */
  const string sep{100, '*'};
  if (0 != enableSyscallStats) {
    ofile << sep << std::endl;
    for (const auto& ac : accessCounter) {
      ofile << "syscall: " <<  setw(15) << sys_info->lookup_activity_name(ac.first) << " " << ac.second;
      if (activityHandlers.find(ac.first) == activityHandlers.end()) {
        ofile << " (not handled)";
      }
      ofile << std::endl;
    }
    ofile << sep << std::endl;
  }
  /* END: SUMMARY */
	
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



OpenFiles* OnlineMonitoringPlugin::findParentFile (const std::shared_ptr<Activity> a) {
  for (const auto aid : a->parentArray()) {
    auto openFile = openFiles.find(aid);
    if (openFiles.end() != openFile) {
      return &openFile->second;
    }
  }
	return nullptr;
}



OpenFiles* OnlineMonitoringPlugin::findParentFileByFh (const std::shared_ptr<Activity> a) {
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



void OnlineMonitoringPlugin::handleSeek (std::shared_ptr<Activity> a) {
  OpenFiles* parent = findParentFileByFh(a);
  assert(nullptr != parent);
  parent->currentPosition = findUINT64AttributeByID(a, positionID[io_iface]);

  uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);
	assert(INVALID_UINT64 != position);

  parent->accesses.push_back(Access{IOAccessType::SEEK, a->time_start_, a->time_stop_, position, 0});
  if (m_tsdb_enabled || m_elastic_enabled) {
    aggregate(IOAccessType::SEEK, a->time_start_, a->time_stop_, position, 0, *parent);
  }
}



void OnlineMonitoringPlugin::handleSync(std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
  assert(nullptr != parent);
	parent->syncOperations.push_back({a->time_start_, a->time_stop_});

	uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);
	assert(INVALID_UINT64 != position);

  parent->accesses.push_back(Access{IOAccessType::SYNC, a->time_start_, a->time_stop_, position, 0});
  if (m_tsdb_enabled || m_elastic_enabled) {
    aggregate(IOAccessType::SYNC, a->time_start_, a->time_stop_, position, 0, *parent);
  }
}



void OnlineMonitoringPlugin::handleWrite (std::shared_ptr<Activity> a) {
	uint64_t bytesWritten = findUINT64AttributeByID(a, bytesWrittenID[io_iface]);
	uint64_t bytesToWrite = findUINT64AttributeByID(a, bytesToWriteID[io_iface]);
	uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);

	OpenFiles* parent = findParentFileByFh(a);
  assert(nullptr != parent);

	if (INVALID_UINT64 == position) {
		position = parent->currentPosition;
		parent->currentPosition += bytesWritten;
	}

  if (0 == a->errorValue() && bytesWritten == bytesToWrite && INVALID_UINT64 != bytesWritten) {
    parent->accesses.push_back(Access{IOAccessType::WRITE, a->time_start_, a->time_stop_, position, bytesWritten});
		if (m_tsdb_enabled || m_elastic_enabled) {
			aggregate(IOAccessType::WRITE, a->time_start_, a->time_stop_, position, bytesWritten, *parent);
		}
  }
}



void OnlineMonitoringPlugin::handleRead (std::shared_ptr<Activity> a) {
	uint64_t bytesRead = findUINT64AttributeByID(a, bytesReadID[io_iface]);
	uint64_t bytesToRead = findUINT64AttributeByID(a, bytesToReadID[io_iface]);
	uint64_t position = findUINT64AttributeByID(a, positionID[io_iface]);

	OpenFiles* parent = findParentFileByFh(a);
  assert(nullptr != parent);

	if (INVALID_UINT64 == position) {
		position = parent->currentPosition;
		parent->currentPosition += bytesRead;
	}

  if (0 == a->errorValue() && bytesRead == bytesToRead && INVALID_UINT64 != bytesRead) {
	  parent->accesses.push_back(Access{IOAccessType::READ, a->time_start_, a->time_stop_, position, bytesRead});
		if (m_tsdb_enabled || m_elastic_enabled) {
			aggregate(IOAccessType::READ, a->time_start_, a->time_stop_, position, bytesRead, *parent);
		}
  }
}



void OnlineMonitoringPlugin::handleOpen (std::shared_ptr<Activity> a) {
  const string name{findStrAttributeByID(a, fname[io_iface])};
  openFiles[a->aid()] = {name, a->time_start_, 0, a->time_stop_ - a->time_start_, 0, 0, a->aid_};
}



void OnlineMonitoringPlugin::handleClose (std::shared_ptr<Activity> a) {
	OpenFiles* parent = findParentFileByFh(a);
	assert(nullptr != parent);
	parent->closeTime = a->time_stop_;
  parent->closeDuration = a->time_stop_ - a->time_start_;
}


extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new OnlineMonitoringPlugin();
	}
}
