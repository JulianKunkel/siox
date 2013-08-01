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
#include <knowledge/optimizer/OptimizerPlugin.hpp>

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
class Optimizer : public core::Component{
public:
	
	/**
	 * Inform the optimizer that @a plugin is able to suggest parameter
	 * values for @a attribute.
	 *
	 * @param attribute [in]
	 * 		The attribute in question
	 * @param plugin [in]
	 *		The plugin that will provide suggestions
	 */
	virtual void registerPlugin(const OntologyAttribute & attribute, const OptimizerPlugin & plugin) = 0;

	/**
	 * Is there a plug-in registered that can provide suggestions for 
	 * @a attribute?
	 *
	 * @param attribute [in]
	 * 		The attribute in question
	 *
	 * @return
	 *		@c true if there is a plug-in regiestered for attribute;
	 *		otherwise @c false.
	 */
	virtual bool isPluginRegistered(const OntologyAttribute & attribute) = 0;

	/**
	 * Remove @a attribute from the optimizer's list of attributes for
	 * which suggestions can be provided.
	 * 
	 * @param attribute [in]
	 * 		The attribute to remove from the list
	 */
	virtual void unregisterPlugin(const OntologyAttribute & attribute) = 0;

	/**
	 * Ask the optimizer to suggest a parameter for @a attribute.
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