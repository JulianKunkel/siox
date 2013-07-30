#ifndef KNOWLEDGE_OPTIMIZER_HPP
#define KNOWLEDGE_OPTIMIZER_HPP

#include <core/component/Component.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>

using namespace monitoring;

namespace knowledge{

class Optimizer : public core::Component{
public:
	virtual OntologyValue optimalParameter(const OntologyAttribute & attribute) = 0;

	virtual bool isPluginRegistered(const OntologyAttribute & attribute) = 0;
	// TODO define type
	//virtual void registerPlugin(const OntologyAttribute & attribute, const OptimizerPlugin & plugin) = 0;
	virtual void unregisterPlugin(const OntologyAttribute & attribute) = 0;
};

}

#define KNOWLEDGE_OPTIMIZER_INTERFACE "knowledge_optimizer"


#endif