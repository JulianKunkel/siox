#ifndef KNOWLEDGE_REASONER_ANOMALYTRIGGER_HPP
#define KNOWLEDGE_REASONER_ANOMALYTRIGGER_HPP

/*
 This interface is triggered by the reasoner, if logging information should be forwarded.
 */
namespace knowledge {

	class AnomalyTrigger {
		public:
			// If the anomaly is still ongoing we keep the excited state.
			virtual void triggerResponseForAnomaly(bool anomalyStillOngoing) = 0;
			virtual void clearAnomalyStatus() = 0;
	};

}


#endif