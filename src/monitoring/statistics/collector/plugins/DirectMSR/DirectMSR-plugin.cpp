/**
 * @author Marc Wiedemann
 * @date 2014
 */

/*
Based on:
Vince Weaver -- vincent.weaver@maine.edu -- 29 November 2013
Romain Dolbeau -- romain@dolbeau.org
on Intel RAPL driver by Zhang Rui <rui.zhang@intel.com>

 The /dev/cpu/??/msr driver must be enabled and permissions set
 to allow read access for this to work. 
*/

#include <stdio.h>
extern "C"{
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
}
#include <monitoring/statistics/collector/StatisticsProviderPluginImplementation.hpp>
#include <monitoring/statistics/collector/plugins/DirectMSR/ProviderDirectMSROptions.hpp>
#include <util/ExceptionHandling.hpp>

#include <vector>

#define MSR_RAPL_POWER_UNIT             0x606

/* Package RAPL Domain */
#define MSR_PKG_RAPL_POWER_LIMIT        0x610
#define MSR_PKG_ENERGY_STATUS           0x611
#define MSR_PKG_PERF_STATUS             0x613
#define MSR_PKG_POWER_INFO              0x614

/* PP0 RAPL Domain */
#define MSR_PP0_POWER_LIMIT             0x638
#define MSR_PP0_ENERGY_STATUS           0x639
#define MSR_PP0_POLICY                  0x63A
#define MSR_PP0_PERF_STATUS             0x63B

/* PP1 RAPL Domain, may reflect to uncore devices */
#define MSR_PP1_POWER_LIMIT             0x640
#define MSR_PP1_ENERGY_STATUS           0x641
#define MSR_PP1_POLICY                  0x642

/* DRAM RAPL Domain */
#define MSR_DRAM_POWER_LIMIT            0x618
#define MSR_DRAM_ENERGY_STATUS          0x619
#define MSR_DRAM_PERF_STATUS            0x61B
#define MSR_DRAM_POWER_INFO             0x61C

/* RAPL UNIT BITMASK */
#define POWER_UNIT_OFFSET       0
#define POWER_UNIT_MASK         0x0F

#define ENERGY_UNIT_OFFSET      0x08
#define ENERGY_UNIT_MASK        0x1F00

#define TIME_UNIT_OFFSET        0x10
#define TIME_UNIT_MASK          0xF000

#define TIME_UNIT_OFFSET        0x10
#define TIME_UNIT_MASK          0xF000


namespace {

	using namespace std;
	using namespace core;
	using namespace monitoring;

class DirectMSRPlugin : public StatisticsProviderPlugin {
		public:
			struct ObservedType{
				const char * unit;
				const char * name;
				bool shouldBeAccumulated;
				StatisticsValue value = 0.0f;
			};

			void init( StatisticsProviderPluginOptions * options ) override;
			void finalize() override;

			ComponentOptions * AvailableOptions() override{
				return new ProviderDirectMSROptions();
			}

			void nextTimestep() throw() override;
			vector<StatisticsProviderDatatypes> availableMetrics() throw() override;
		private:
			void DirectMSRDerivedEventToOntology(const char * DirectMSRName, char const ** outName, char const ** outUnit, bool * shouldBeAccumulated);

			// results from DirectMSR
			double * values;
			vector<ObservedType> statistics;
			int numCores;
	};

	void DirectMSRPlugin::finalize(void){
		perfmon_stopCounters();
		perfmon_finalize();
	}

	void DirectMSRPlugin::init( StatisticsProviderPluginOptions * sp_options ) {
		ProviderDirectMSROptions * options = dynamic_cast<ProviderDirectMSROptions*>( sp_options);

		 printf("\n");

  		 opterr=0;

  		 while ((c = getopt (argc, argv, "c:")) != -1) {
    	 switch (c)
    	  {
    	  case 'c':
      	  core = atoi(optarg);
      	  break;
    	  default:
      	  exit(-1);
    	  }
  		 }

        cpu_model=detect_cpu();
        if (cpu_model<0) {
         printf("Unsupported CPU type\n");
         return -1;
        }

        printf("Checking core #%d\n",core);

		if (cpuid_init() == EXIT_FAILURE){
			assert(FALSE && "CPUID_INIT");
		}
		numa_init();
		//timer_init();


		const int numThreads = cpuid_topology.numSockets * cpuid_topology.numCoresPerSocket * cpuid_topology.numThreadsPerCore;
		numCores = numThreads;
		int * threads = (int*) malloc(sizeof(int) * numThreads);
		for(int i=0; i < numThreads; i++){
			threads[i] = i;
		}

		// 0 or 1 at the moment
		accessClient_setaccessmode(1);

		FILE * nullFD = fopen("/dev/null", "w");
		perfmon_init(numThreads, threads, nullFD);
		/*
		int groupCount = options->groups.length() > 0 ? 1 : 0;
		const char * cur = options->groups.c_str();
		while( *cur != '\0'){
			if ( *cur == ',') groupCount++;
			cur++;
		}

		// create all groups
		// TODO support multiple groups properly
		assert(groupCount == 1);
		DirectMSRSetup = perfmon_prepareEventSetup( (char*) options->groups.c_str());

		values = (double*) malloc( DirectMSRSetup.numberOfDerivedCounters * sizeof(double) * 3);
		statistics.resize(DirectMSRSetup.numberOfDerivedCounters);

		for( int i=0; i < DirectMSRSetup.numberOfDerivedCounters; i++ ){			
			// extract correct unit from DirectMSR
			DirectMSRDerivedEventToOntology(DirectMSRSetup.derivedNames[i], & statistics[i].name, & statistics[i].unit, & statistics[i].shouldBeAccumulated);
		}
    */
		perfmon_setupCountersForEventSet(& DirectMSRSetup);
		perfmon_startCounters();		
	}

	void DirectMSRPlugin::nextTimestep() throw() {
		//perfmon_stopCounters();
		DirectMSRPlugin::run();
		//perfmon_getDerivedCounterValues(& values[0], & values[DirectMSRSetup.numberOfDerivedCounters], & values[DirectMSRSetup.numberOfDerivedCounters*2]);	
		
    statistics[1].value = values[1];
    statistics[2].value = values[2];
    /*
		// copy DirectMSR results
		for( int i=0; i < DirectMSRSetup.numberOfDerivedCounters; i++ ){
			if ( statistics[i].shouldBeAccumulated ){				
				statistics[i].value = values[i] * numCores;
				//cout << statistics[i].name << " " << statistics[i].value << " " << numCores << endl;
			}else{
				statistics[i].value = values[i];
			}
		}*/

		//perfmon_startCounters();
	}

 void DirectMSRPlugin::DirectMSRDerivedEventToOntology(const char * DirectMSRName, char const ** outName, char const ** outUnit, bool * shouldBeAccumulated){
		struct DirectMSRType{
			const char * Lname;
			const char * unit;
			const char * name;
			bool shouldBeAccumulated;
    		
		};

		DirectMSRType types[] = {
			{"Energy [J]", "J", "energy/Socket/Direct", true},
			{"Energy RAM [J]", "J", "energy/RAM/Direct", true},
			{"Power [W]", "W", "power/Direct", true},
			{nullptr, nullptr, nullptr} };
		const DirectMSRType * check_type = types;

		//FIXME: Undefined behaviour when the DirectMSRName is not found in the list (*outName and *outUnit remain uninitialized).
		while(check_type->Lname != nullptr){
			if (strcmp(check_type->Lname, DirectMSRName) == 0){
				*outName = check_type->name;
				*outUnit = check_type->unit;
				*shouldBeAccumulated = check_type->shouldBeAccumulated;
				return;
			}
			check_type++;
		}

		cerr << "DirectMSRPlugin: Warning do not understand event: " << DirectMSRName << endl;
	}

	vector<StatisticsProviderDatatypes> DirectMSRPlugin::availableMetrics() throw() {
		vector<StatisticsProviderDatatypes> result;

		#define addGaugeMetric( name, variableName, unitString, descriptionString ) \
			result.push_back( {name, "@localhost", variableName, GAUGE, unitString, descriptionString, 0, 0} ); 
		#define addIncrementalMetric( name, variableName, unitString, descriptionString ) \
			result.push_back( {name, "@localhost", variableName, INCREMENTAL, unitString, descriptionString, 0, 0} ); 


		for( int i=0; i < DirectMSRSetup.numberOfDerivedCounters; i++ ){			
			// TODO use correct name for the metric.	
			addGaugeMetric( statistics[i].name, statistics[i].value, statistics[i].unit, DirectMSRSetup.derivedNames[i]);
      addGaugeMetric( statistics[i].Ram_energy, statistics[i].value, statistics[i].energy_units, RamEnergy);
      addGaugeMetric( statistics[i].Core_energy, statistics[i].value, statistics[i].energy_units, CoreEnergy);
      addGaugeMetric( statistics[i].Socket_energy, statistics[i].value, statistics[i].energy_units, SocketEnergy);
		}

		return result;
	}
 }

 int DirectMSRPlugin::open_msr(int core) {
     char msr_filename[BUFSIZ];
     int fd;

     sprintf(msr_filename, "/dev/cpu/%d/msr", core);
     fd = open(msr_filename, O_RDONLY);
     if ( fd < 0 ) {
      if ( errno == ENXIO ) {
        fprintf(stderr, "rdmsr: No CPU %d\n", core);
        exit(2);
      } else if ( errno == EIO ) {
        fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n", core);
        exit(3);
      } else {
        perror("rdmsr:open");
        fprintf(stderr,"Trying to open %s\n",msr_filename);
      exit(127);
    }
  }
  return fd;
 }

 long long DirectMSRPlugin::read_msr(int fd, int which) {

  uint64_t data;

  if ( pread(fd, &data, sizeof data, which) != sizeof data ) {
    perror("rdmsr:pread");
    exit(127);
  }
  return (long long)data;
 }

#define CPU_SANDYBRIDGE         42
#define CPU_SANDYBRIDGE_EP      45
#define CPU_IVYBRIDGE           58
#define CPU_IVYBRIDGE_EP        62
#define CPU_HASWELL             60

 int DirectMSRPlugin::detect_cpu(void) {

        FILE *fff;

        int family,model=-1;
        char buffer[BUFSIZ],*result;
        char vendor[BUFSIZ];

        fff=fopen("/proc/cpuinfo","r");
        if (fff==NULL) return -1;

        while(1) {
                result=fgets(buffer,BUFSIZ,fff);
                if (result==NULL) break;

                if (!strncmp(result,"vendor_id",8)) {
                        sscanf(result,"%*s%*s%s",vendor);

                        if (strncmp(vendor,"GenuineIntel",12)) {
                                printf("%s not an Intel chip\n",vendor);
                                return -1;
                        }
                }

                if (!strncmp(result,"cpu family",10)) {
                        sscanf(result,"%*s%*s%*s%d",&family);
                        if (family!=6) {
                                printf("Wrong CPU family %d\n",family);
                                return -1;
                        }
                }

                if (!strncmp(result,"model",5)) {
                        sscanf(result,"%*s%*s%d",&model);
                }

        }

        fclose(fff);

        switch(model) {
                case CPU_SANDYBRIDGE:
                        printf("Found Sandybridge CPU\n");
                        break;
                case CPU_SANDYBRIDGE_EP:
                        printf("Found Sandybridge-EP CPU\n");
                        break;
                case CPU_IVYBRIDGE:
                        printf("Found Ivybridge CPU\n");
                        break;
                case CPU_IVYBRIDGE_EP:
                        printf("Found Ivybridge-EP CPU\n");
                        break;
                case CPU_HASWELL:
                        printf("Found Haswell CPU\n");
                        break;
                default:        printf("Unsupported model %d\n",model);
                                model=-1;
                                break;
        }
        return model;
 }

 int DirectMSRPlugin::run(int argc, char **argv) {
    int fd;
  	int core=0;
  	long long result;
  	double power_units,energy_units,time_units;
  	double package_before,package_after;
  	double pp0_before,pp0_after;
  	double pp1_before=0.0,pp1_after;
  	double dram_before=0.0,dram_after;
  	double thermal_spec_power,minimum_power,maximum_power,time_window;
  	int cpu_model;
  	int c;
  	
  fd=DirectMSRPlugin::open_msr(core);

  /* Calculate the units used */
  result=DirectMSRPlugin::read_msr(fd,MSR_RAPL_POWER_UNIT);
  
  power_units=pow(0.5,(double)(result&0xf));
  energy_units=pow(0.5,(double)((result>>8)&0x1f));
  time_units=pow(0.5,(double)((result>>16)&0xf));

  printf("Power units = %.3fW\n",power_units);
  printf("Energy units = %.8fJ\n",energy_units);
  printf("Time units = %.8fs\n",time_units);
  printf("\n");

  /* Show package power info */
  result=DirectMSRPlugin::read_msr(fd,MSR_PKG_POWER_INFO);
  thermal_spec_power=power_units*(double)(result&0x7fff);
  printf("Package thermal spec: %.3fW\n",thermal_spec_power);
  minimum_power=power_units*(double)((result>>16)&0x7fff);
  printf("Package minimum power: %.3fW\n",minimum_power);
  maximum_power=power_units*(double)((result>>32)&0x7fff);
  printf("Package maximum power: %.3fW\n",maximum_power);
  time_window=time_units*(double)((result>>48)&0x7fff);
  printf("Package maximum time window: %.6fs\n",time_wicd /ndow);

  /* Show package power limit */
  result=DirectMSRPlugin::read_msr(fd,MSR_PKG_RAPL_POWER_LIMIT);
    printf("Package power limits are %s\n", (result >> 63) ? "locked" : "unlocked");
  double pkg_power_limit_1 = power_units*(double)((result>>0)&0x7FFF);
  double pkg_time_window_1 = time_units*(double)((result>>17)&0x007F);
  printf("Package power limit #1: %.3fW for %.6fs (%s, %s)\n", pkg_power_limit_1, pkg_time_window_1,
           (result & (1LL<<15)) ? "enabled" : "disabled",
           (result & (1LL<<16)) ? "clamped" : "not_clamped");
  double pkg_power_limit_2 = power_units*(double)((result>>32)&0x7FFF);
  double pkg_time_window_2 = time_units*(double)((result>>49)&0x007F);
  printf("Package power limit #2: %.3fW for %.6fs (%s, %s)\n", pkg_power_limit_2, pkg_time_window_2,
          (result & (1LL<<47)) ? "enabled" : "disabled",
          (result & (1LL<<48)) ? "clamped" : "not_clamped");

  printf("\n");

  /* result=read_msr(fd,MSR_RAPL_POWER_UNIT); */

  result=DirectMSRPlugin::read_msr(fd,MSR_PKG_ENERGY_STATUS);
    package_before=(double)result*energy_units;
  printf("Package energy before: %.6fJ\n",package_before);

  /* only available on *Bridge-EP */
  if ((cpu_model==CPU_SANDYBRIDGE_EP) || (cpu_model==CPU_IVYBRIDGE_EP))
  {
    result=DirectMSRPlugin::read_msr(fd,MSR_PKG_PERF_STATUS);
    double acc_pkg_throttled_time=(double)result*time_units;
    printf("Accumulated Package Throttled Time : %.6fs\n",acc_pkg_throttled_time);
  }

  result=DirectMSRPlugin::read_msr(fd,MSR_PP0_ENERGY_STATUS);
  pp0_before=(double)result*energy_units;
  printf("PowerPlane0 (core) for core %d energy before: %.6fJ\n",core,pp0_before);

  result=DirectMSRPlugin::read_msr(fd,MSR_PP0_POLICY);
  int pp0_policy=(int)result&0x001f;
    printf("PowerPlane0 (core) for core %d policy: %d\n",core,pp0_policy);

  /* only available on *Bridge-EP */
  if ((cpu_model==CPU_SANDYBRIDGE_EP) || (cpu_model==CPU_IVYBRIDGE_EP))
  {
    result=DirectMSRPlugin::read_msr(fd,MSR_PP0_PERF_STATUS);
    double acc_pp0_throttled_time=(double)result*time_units;
    printf("PowerPlane0 (core) Accumulated Throttled Time : %.6fs\n",acc_pp0_throttled_time);
  }

  /* not available on *Bridge-EP */
  if ((cpu_model==CPU_SANDYBRIDGE) || (cpu_model==CPU_IVYBRIDGE) ||
        (cpu_model==CPU_HASWELL)) {
     result=DirectMSRPlugin::read_msr(fd,MSR_PP1_ENERGY_STATUS);
     pp1_before=(double)result*energy_units;
     printf("PowerPlane1 (on-core GPU if avail) before: %.6fJ\n",pp1_before);
     result=DirectMSRPlugin::read_msr(fd,MSR_PP1_POLICY);
     int pp1_policy=(int)result&0x001f;
     printf("PowerPlane1 (on-core GPU if avail) %d policy: %d\n",core,pp1_policy);
  }

        /* Despite documentation saying otherwise, it looks like */
        /* You can get DRAM readings on regular Haswell          */
  if ((cpu_model==CPU_SANDYBRIDGE_EP) || (cpu_model==CPU_IVYBRIDGE_EP) ||
        (cpu_model==CPU_HASWELL)) {
     result=DirectMSRPlugin::read_msr(fd,MSR_DRAM_ENERGY_STATUS);
     dram_before=(double)result*energy_units;
     printf("DRAM energy before: %.6fJ\n",dram_before);
  }

  printf("\nSleeping 1 second\n\n");
  sleep(1);
  
  /* Socket energy (package) */
  result=DirectMSRPlugin::read_msr(fd,MSR_PKG_ENERGY_STATUS);
  package_after=(double)result*energy_units;
  printf("Package energy after: %.6f  (%.6fJ consumed)\n",
         package_after,package_after-package_before);
         Socket_energy=package_after-package_before;

  /* CPU core (Power Plane 0) */
  result=DirectMSRPlugin::read_msr(fd,MSR_PP0_ENERGY_STATUS);
  pp0_after=(double)result*energy_units;
  printf("PowerPlane0 (core) for core %d energy after: %.6f  (%.6fJ consumed)\n",
         core,pp0_after,pp0_after-pp0_before);

  /* not available on SandyBridge-EP */
  if ((cpu_model==CPU_SANDYBRIDGE) || (cpu_model==CPU_IVYBRIDGE) ||
        (cpu_model==CPU_HASWELL)) {
     result=DirectMSRPlugin::read_msr(fd,MSR_PP1_ENERGY_STATUS);
     pp1_after=(double)result*energy_units;
     printf("PowerPlane1 (on-core GPU if avail) after: %.6f  (%.6fJ consumed)\n",
         pp1_after,pp1_after-pp1_before);
     Core_energy=pp1_after-pp1_before;
  }

  if ((cpu_model==CPU_SANDYBRIDGE_EP) || (cpu_model==CPU_IVYBRIDGE_EP) ||
        (cpu_model==CPU_HASWELL)) {
     result=DirectMSRPlugin::read_msr(fd,MSR_DRAM_ENERGY_STATUS);
     dram_after=(double)result*energy_units;
     printf("DRAM energy after: %.6f  (%.6fJ consumed)\n",
         dram_after,dram_after-dram_before);
     Ram_energy=dram_after-dram_before;
  }

  printf("\n");
  printf("Note: the energy measurements can overflow in 60s or so\n");
  printf("      so try to sample the counters more often than that.\n\n");
  close(fd);

  return 0;
 }

//namespace end
}


extern "C" {
	void * MONITORING_STATISTICS_PLUGIN_INSTANCIATOR_NAME() {
		return new DirectMSRPlugin();
	}
}

