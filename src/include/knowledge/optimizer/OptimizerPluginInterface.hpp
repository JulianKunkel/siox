/**
* @file OptimizerPluginInterface.hpp
* Abstract OptimizerPluginInterface class
* @author Julian Kunkel, Michaela Zimmer
* @date 2013-11-04
*/

#ifndef KNOWLEDGE_OPTIMIZER_PLUGIN_HPP
#define KNOWLEDGE_OPTIMIZER_PLUGIN_HPP

#include <core/component/Component.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/datatypes/Exceptions.hpp>

// #include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace core;
using namespace monitoring;

namespace knowledge { 

	/**
	 * A plug-in that will suggest optimal parameter values for one or more
	 * OntologyAttribute types.
	 *
	 * Any implementation of this class, unless creating suggestions without
	 * independent of the current system state, will also have to inherit from
	 * either one or both of the ActivityMultiplexerPlugin and
	 * StatisticsMultiplexerPlugin classes.
	 *
	 * Upon initialization, the plugin has to register for which attributes
	 * it can provide suggestions, and should unregister these as well when
	 * shutting down:
	 *
	    @startuml{Sequence-Optimizer.png}

	    participant "Self-Optimization-\nPlug-In" as sopi #Plum
	    participant "Optimizer" as opt #Plum
	    participant "siox-ll" as ll #SkyBlue

	    [-> sopi : [<i>Address of Optimizer</i>]

	    loop for every Attribute handled
	        sopi -> opt : registerPlugin( PlugIn,  Attribute )
	        sopi <-- opt
	    end

	    loop while SOPI active
	        opt <- ll : optimalParameter( Attribute )
	        sopi <- opt : optimalParameter( Attribute )
	        sopi --> opt : value
	        opt --> ll : value
	    end

	    loop for every Attribute handled
	        sopi -> opt : unregisterPlugin( PLugIn )
	        sopi <-- opt
	    end
	    @enduml
	 */
	class OptimizerInterface{
		public:
			/**
			 * Ask optimizer plugin to suggest a parameter for attribute.
			 *
			 * @param attribute [in]
			 *      The attribute in question
			 *
			 * @return
			 *      The best value for attribute, as judged by the plugin.
			 */
			virtual OntologyValue optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) = 0;
	};

}

#define KNOWLEDGE_OPTIMIZER_PLUGIN_INTERFACE "knowledge_optimizer_plugin"


#endif