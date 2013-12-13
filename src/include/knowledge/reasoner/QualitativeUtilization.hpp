#ifndef KNOWLEDGE_REASONER_QUALITATIVE_UTILIZATION_HPP
#define KNOWLEDGE_REASONER_QUALITATIVE_UTILIZATION_HPP

#include <monitoring/datatypes/Exceptions.hpp>
#include <monitoring/datatypes/ids.hpp>

#include <knowledge/reasoner/ReasoningDatatypes.hpp>

/*
 This interface is pulled by the reasoner on demand to query relative utilization.
 */
namespace knowledge {

	class QualitativeUtilization {
		public:
			// Return the current observation for the selected datatype
			virtual uint8_t lastUtilization( monitoring::OntologyAttributeID id ) const throw( NotFoundError ) = 0;
	};

}

#endif