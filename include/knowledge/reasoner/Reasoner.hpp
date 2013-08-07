#ifndef KNOWLEDGE_REASONER_HPP
#define KNOWLEDGE_REASONER_HPP

#include <core/component/Component.hpp>


/**
  The reasoner gathers all the information about anomalies and judges the cause/reason of them. 
  It knows the system utilization and tries to resolve 1) local and 2) global anomalies.
  If the source of performance degradation is not known, logging will be enabled by the reasoner.
  
  Use cases:
  U1) 
  
  Requirements:
  R1) 
 
  Rationales & design decisions:
  D1) 

 */
namespace knowledge{

class Reasoner : public core::Component{
public:

};

}

#define KNOWLEDGE_REASONER_INTERFACE "knowledge_reasoner"

#endif
