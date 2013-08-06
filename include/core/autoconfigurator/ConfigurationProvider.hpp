#ifndef SIOX_CONFIGURATION_PROVIDER_H
#define SIOX_CONFIGURATION_PROVIDER_H

#include <string>
#include <map>

namespace core{

/*
 * The ConfigurationProvider provides a configuration with all relevant information for this process.
 * This may include sections that are never used by the current process, e.g. monitoring configurations of layers that are not used by the process.
 *
 * //////////////////////// Use cases ///////////////////////////
 * The global configuration contains sections which can be queried independently.
 * A section contains a node-specific configuration which has the following structure:
 * <Configuration type="daemon" hostname="X" [instance="<instance-name>"]>
 * </Configuration>
 * To allow multiple deamons on a node and configurations an instance/configuration name is provided.
 * 
 * To configure each instrumented interface we have:
 * <Configuration type="layer" interface=<interface-name> [hostname="X"]  [implementation=<implementation-name>]>
 * </Configuration>
 * e.g. <Configuration type="layer" interface="MPIv2" hostname="p249" implementation="MPICH3.0">
 * Thus it is possible for each version of the implementation to have its own configuration.
 * The interface is mandatory, while the hostname and implementation are optional.
 * Priority is to match interface and if possible implementation, then if possible match hostname.
 * 
 * //////////////////////// Design decisions ///////////////////////////
 * How to deal with the different required classifiers (hostname, interface, etc.) but still keep a general approach?
 * 1) A map with matching critera can be submitted, the configuration tries to match all optional key/value pairs if possible.
 *
 * Since this class does provide the configuration it does not make sense to be derived from Component.
 * Instead its elementary settings (e.g. where to fetch the config) must be read from either a well-defined configuration file, or from environment variables.
 * For testing purpose and manual override a constructur in child classes permits to set the elementary setting, but this is module-specific.
 */
class ConfigurationProvider{
public:
	/*
	 * The configuration entry_point describes where to get the basic information, e.g. file or DB / demon connection to use for fetching the info.
	 * An implementation may use the hwid to request only relevant configuration pieces.
	 */
	virtual void connect(std::string & configuration_entry_point) = 0;

	virtual const std::string & getConfiguration(std::string & type, std::string & matchingRules) = 0;

	virtual ~ConfigurationProvider() {};
};

}

#define CONFIGURATION_PROVIDER_INTERFACE "core_configuration_provider"


#endif
