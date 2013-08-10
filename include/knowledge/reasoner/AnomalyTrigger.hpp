#ifndef KNOWLEDGE_REASONER_ANOMALYTRIGGER_HPP
#define KNOWLEDGE_REASONER_ANOMALYTRIGGER_HPP

/*
 This interface is triggered by the reasoner, if logging information should be forwarded.
 */
namespace knowledge{

class AnomalyTrigger{
public:
	void triggerResponseForAnomaly();
};

}


#endif