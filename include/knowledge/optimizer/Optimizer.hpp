/**
* @file Optimizer.hpp
* Abstract Optimizer class
* @author Julian Kunkel, Michaela Zimmer
* @date 2013-07-31
*/

#ifndef KNOWLEDGE_OPTIMIZER_HPP
#define KNOWLEDGE_OPTIMIZER_HPP

#include <core/component/Component.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPlugin.hpp>

using namespace monitoring;

namespace knowledge{

/**
 * A central access point that will distribute a component's queries
 * for optimal parameter value suggestions to a variety of 
 * SelfOptimizationPlugIn instances.
 *
 * The plugins have to register for which attributes they can provide
 * suggestions, and should unregister these as well when shutting
 * down: 
 *
	@startuml{Sequence-Optimizer.png}

	participant "Self-Optimization-\nPlug-In" as sopi #Plum
	participant "Optimizer" as opt #Plum
	participant "siox-ll" as ll #SkyBlue

	[-> sopi : [<i>Address of Optimizer</i>]

	loop for every Attribute handled
		sopi -> opt : register( PlugIn,  Attribute )
		sopi <-- opt
	end

	loop while SOPI active
		opt <- ll : suggest_optimization( Attribute )
		sopi <- opt : current_best_value( Attribute )
		sopi --> opt : value
		opt --> ll : value
	end 

	loop for every Attribute handled
		sopi -> opt : unregister( PLugIn )
		sopi <-- opt
	end
	@enduml
 */
class Optimizer : public core::Component{
public:
	
	/**
	 * Indicate that plugin is able to suggest parameter
	 * values for attribute.
	 *
	 * @param attribute [in]
	 * 		The attribute in question
	 * @param plugin [in]
	 *		The plugin that will provide suggestions
	 *
	 * @todo Define type?
	 */
	virtual void registerPlugin(const OntologyAttribute & attribute, const ActivityMultiplexerPlugin & plugin) = 0;

	/**
	 * Is there a plugin registered that can provide suggestions for 
	 * attribute?
	 *
	 * @param attribute [in]
	 * 		The attribute in question
	 *
	 * @return
	 *		@true if there is a plug-in regiestered for attribute;
	 *		otherwise @false.
	 */
	virtual bool isPluginRegistered(const OntologyAttribute & attribute) = 0;

	/**
	 * Remove attribute from optimizer's list of attributes for
	 * which suggestions exist.
	 * 
	 * @param attribute [in]
	 * 		The attribute to remove from the list
	 *
	 * @todo Why not call with plugin as parameter?!
	 */
	virtual void unregisterPlugin(const OntologyAttribute & attribute) = 0;

	/**
	 * Ask optimizer to suggest a parameter for attribute.
	 *
	 * @param attribute [in]
	 * 		The attribute in question
	 *
	 * @return
	 *		The best value for attribute, as judged by the plugin
	 *		registered for attribute with the optimizer, based
	 *		upon current system statistics and activities.
	 */
	virtual OntologyValue optimalParameter(const OntologyAttribute & attribute) = 0;
};

}

#define KNOWLEDGE_OPTIMIZER_INTERFACE "knowledge_optimizer"


#endif