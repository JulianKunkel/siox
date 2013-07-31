/**
* @file OptimizerPlugin.hpp
* Abstract OptimizerPlugin class
* @author Julian Kunkel, Michaela Zimmer
* @date 2013-07-31
*/

#ifndef KNOWLEDGE_OPTIMIZER_PLUGIN_HPP
#define KNOWLEDGE_OPTIMIZER_PLUGIN_HPP

#include <core/component/Component.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
// #include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace core;
using namespace monitoring;

namespace knowledge{

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
	@startuml{Sequence-OptimizerPlugin.png}

	participant "Optimizer-\nPlug-In" as sopi #Plum
	participant "Optimizer" as opt #Plum
	participant "siox-ll" as ll #SkyBlue

	[-> sopi : [<i>Address of Optimizer</i>]

	loop for every Attribute handled
		sopi -> opt : registerPlugin( PlugIn,  Attribute )
		sopi <-- opt
	end

	loop while SOPI active
		opt <- ll : suggest_optimization( Attribute )
		sopi <- opt : current_best_value( Attribute )
		sopi --> opt : value
		opt --> ll : value
	end 

	loop for every Attribute handled
		sopi -> opt : unregisterPlugin( PLugIn )
		sopi <-- opt
	end
	@enduml
 */
class OptimizerPlugin : public Component{

protected:
	// Optimizer * parent_optimizer;
	// OptimizerPluginDereferencing * dereferenceFacade;

	// virtual void init(OptimizerPluginOptions * options, Optimizer & optimizer) = 0;

public:
/*	void init(OptimizerPluginOptions * options, Optimizer * optimizer, StatisticsPluginDereferencing * dereferenceFacade){
		parent_optimizer = optimizer;
		// may be 0.
		this->dereferenceFacade = dereferenceFacade;

		init(options, *parent_optimizer );		
	}

	void init(ComponentOptions * options){
		OptimizerPluginOptions * o = (OptimizerPluginOptions *) options;
		assert(options != nullptr);
		assert(o->multiplexer.componentID != 0);

		init(o, o->multiplexer.instance<Optimizer>(), o->dereferenceFacade.instance<StatisticsPluginDereferencing>());
	}

	void shutdown(){
	}
*/

	/**	
	 * Ask optimizer plugin to suggest a parameter for attribute.
	 *
	 * @param attribute [in]
	 * 		The attribute in question
	 *
	 * @return
	 *		The best value for attribute, as judged by the plugin.
	 */
	virtual OntologyValue optimalParameter(const OntologyAttribute & attribute) = 0;
};

}

#define KNOWLEDGE_OPTIMIZER_PLUGIN_INTERFACE "knowledge_optimizer_plugin"


#endif