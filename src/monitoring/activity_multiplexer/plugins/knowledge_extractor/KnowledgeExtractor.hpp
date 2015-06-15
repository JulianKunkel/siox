/*
 * =====================================================================================
 *
 *       Filename:  AMumPlugin.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/09/2015 02:00:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
// Features: duration, datasize, 
// Label: Performance

#ifndef KNOWLEDGEEXTRACTOR_HPP
#define KNOWLEDGEEXTRACTOR_HPP

#include <unordered_map>

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginImplementation.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <core/reporting/ComponentReportInterface.hpp>
#include <knowledge/reasoner/AnomalyPlugin.hpp>

//#include <NeuralNetwork.hpp>
#include "classification/Classifier.hpp"

//#include "ActivityStore.hpp"
#include "KnowledgeExtractorOptions.hpp"
#include "predictor/CART.hpp"



namespace kep {
	/**
	 * @brief Learning: learn from activities -> train model -> store model
	 * Prediction: compute overall time -> compute predicted time -> print result
	 */
	enum class Mode {Training, Prediction};
// public core::ComponentReportInterface, public knowledge::AnomalyPlugin {


	struct ClassStats {
		size_t counter; 
		double sumReal;  
		double sumPredicted;
	};

	class KnowledgeExtractor : public monitoring::ActivityMultiplexerPlugin, public core::ComponentReportInterface, public knowledge::AnomalyPlugin {
		public:
			using FeatureType = double;
			using FeatureVector = std::vector<FeatureType>;
			using FeatureSet = std::vector<FeatureVector>;
			using LabelType = double;				
			using LabelSet = std::vector<LabelType>;
			using Predictor = ml::alg::CART<FeatureVector, LabelType>;

			using ClassifierLabelType = unsigned int;
			using ClassifierLabelSet = std::vector<ClassifierLabelType>;
			using Classifier = ml::alg::Classifier<FeatureVector, ClassifierLabelType>;

			KnowledgeExtractor();
			virtual ~KnowledgeExtractor();


			void notify(const shared_ptr<Activity>& activity, int lost);
			void notifyPredictionInit(const shared_ptr<Activity>& activity, int lost);
			void notifyPrediction(const shared_ptr<Activity>& activity, int lost);
			void notifyTrainingInit(const shared_ptr<Activity>& activity, int lost);
			void notifyTraining(const shared_ptr<Activity>& activity, int lost);
			void setNotifyFunction();


			void initPlugin() override;
			//		void start() override;
			//		void stop() override;
			void setMode(const Mode mode);
			Mode getMode() const {return m_mode;}
			void trainPredictor();
			void trainClassifier();

			ComponentOptions* AvailableOptions() override;
			virtual ComponentReport prepareReport() override;
//		 	{
//				using namespace std;
//				cout << __PRETTY_FUNCTION__ << endl;
//				return ComponentReport{};
//			}

				void finalize() override;
		private:
				SystemInformationGlobalIDManager* m_sysinfo = nullptr;
				UniqueInterfaceID m_uiid;
				std::unordered_map<std::string, size_t> m_system_call_counter;
//				CART<std::vector<double>, double>  m_predictor;

				OntologyAttributeID m_uidAttrID; // user-id
				OntologyAttributeID m_fnAttrID;  // file name
				OntologyAttributeID m_fpAttrID;  // file position
				OntologyAttributeID m_btrAttrID; // bytes to read
				OntologyAttributeID m_btwAttrID; // bytes to write
				OntologyAttributeID m_brAttrID;  // bytes read
				OntologyAttributeID m_bwAttrID;  // bytes written

				
				void (KnowledgeExtractor::*m_notifyFunction) (const shared_ptr<Activity>& activity, int lost);
				const Attribute* findAttributeByID(const std::shared_ptr<Activity>& activity, OntologyAttributeID oaid);
				const ActivityID* findParentAID(const std::shared_ptr<Activity>& activity);
				Predictor::Sample constructSample(const std::shared_ptr<Activity>& activity);
				
				FeatureSet m_features;
				LabelSet m_labels;				
				LabelSet m_predicted_labels;
				ClassifierLabelSet m_predicted_categories;
				Predictor m_predictor;
				Classifier m_classifier;

				std::shared_ptr<Activity> m_prevActivity;
				Mode m_mode;
				bool m_visualize;
				std::string m_predictorName;
				std::string m_classifierName;
				std::string m_outputName;
				std::ofstream m_outputFile;
				size_t m_numOfClasses;
				size_t m_anomalyCounter = 0;

				long m_overallTime;
				long m_overallPredictedTime;
				std::unordered_map<UniqueComponentActivityID, size_t> m_accessCounter;
				std::unordered_map<size_t, ClassStats> m_classStats; 
	};

#endif
}		/* -----  end of namespace kep  ----- */
