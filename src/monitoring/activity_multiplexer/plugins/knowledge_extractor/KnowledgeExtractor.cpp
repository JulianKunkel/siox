/*
 * =====================================================================================
 *
 *       Filename:  KnowledgeExtractor.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  03/09/2015 02:03:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "KnowledgeExtractor.hpp"
#include <util/ExceptionHandling.hpp>
#include <random>
#include <chrono>
#include <cassert>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <iomanip>

namespace kep {


	/**
	 * @brief Default constructor
	 */
	KnowledgeExtractor::KnowledgeExtractor() : 
		m_prevActivity{nullptr}
	{}



	/**
	 * @brief Destructor
	 */
	KnowledgeExtractor::~KnowledgeExtractor() {
		m_outputFile.close();
	}



	/**
	 * @brief Construct training sample
	 * size: bytes
	 * duration: 
	 * offset:  
	 * operation type: read, seek, write
	 *
	 *
	 * @param activity
	 *
	 * @return 
	 */
	KnowledgeExtractor::Predictor::Sample KnowledgeExtractor::constructSample(const std::shared_ptr<Activity>& activity) {
		Predictor::Sample sample;
		Predictor::F& featureVector = sample.first;
		Predictor::L& label = sample.second;

		/* ===== feature values ======= */
			// size
			double bytes = 0;
			const std::vector<Attribute>& attributes = activity->attributeArray();
			for (const Attribute& attribute : attributes) {
				if (attribute.id == m_bwAttrID || attribute.id == m_brAttrID) {
					bytes = attribute.value.toDouble();
				}
			}

			// duration
			const Timestamp duration = activity->duration();

			// offset
			Timestamp offset = 0;
			assert(m_prevActivity != nullptr);
			offset = activity->time_start() - m_prevActivity->time_stop();
			
			// operation type
			assert(activity->attributeArray().size() >= 1);
			const UniqueComponentActivityID accessType = activity->attributeArray()[0].id;
		/* ====== feature values =======  */
	
		m_prevActivity = activity;
		featureVector.push_back(static_cast<FeatureType>(bytes));
		featureVector.push_back(static_cast<FeatureType>(duration));
		featureVector.push_back(static_cast<FeatureType>(offset));
		featureVector.push_back(static_cast<FeatureType>(accessType));

		// throughput
		label = bytes / duration;
		return sample;
	}



	/**
	 * @brief 
	 *
	 * @param mode
	 */
	void KnowledgeExtractor::setMode(const Mode mode) {
		using namespace std;
		cout << __PRETTY_FUNCTION__	<< endl;
		m_mode = mode;
		switch (m_mode) {
			case Mode::Training:
				m_notifyFunction = &KnowledgeExtractor::notifyTrainingInit;
				break;
			case Mode::Prediction:
				m_notifyFunction = &KnowledgeExtractor::notifyPredictionInit;
				break;
		}
	}



	/**
	 * @brief
	 */
	void KnowledgeExtractor::setNotifyFunction() {
		using namespace std;
		cout << __PRETTY_FUNCTION__	<< endl;
		KnowledgeExtractorOptions& o = getOptions<KnowledgeExtractorOptions>();
		string interface = o.interface;
		string implementation = o.implementation;
		RETURN_ON_EXCEPTION( m_uiid = m_sysinfo->lookup_interfaceID(interface, implementation); );


		if (multiplexer == nullptr) {
			cout << __PRETTY_FUNCTION__ << " : " << "multiplexer == nullptr" << endl;
			exit(1);
		}

		for (const auto& token : o.seekTokens) {
			UniqueInterfaceID ucaid = m_sysinfo->lookup_activityID(m_uiid, token);
			multiplexer->registerForUcaid(ucaid, this, static_cast<ActivityMultiplexer::Callback>(&KnowledgeExtractor::notify), false);
		}
		for (const auto& token : o.readTokens) {
			UniqueInterfaceID ucaid = m_sysinfo->lookup_activityID(m_uiid, token);
			multiplexer->registerForUcaid(ucaid, this, static_cast<ActivityMultiplexer::Callback>(&KnowledgeExtractor::notify), false);
		}
		for (const auto& token : o.writeTokens) {
			UniqueInterfaceID ucaid = m_sysinfo->lookup_activityID(m_uiid, token);
			multiplexer->registerForUcaid(ucaid, this, static_cast<ActivityMultiplexer::Callback>(&KnowledgeExtractor::notify), false);
		}
	}



	/**
	 * @brief 
	 */
	void KnowledgeExtractor::initPlugin() {
		using namespace std;
		cout << __PRETTY_FUNCTION__	<< endl;
		KnowledgeExtractorOptions& o = getOptions<KnowledgeExtractorOptions>();
		m_sysinfo = facade->get_system_information();
		assert(m_sysinfo);

		setNotifyFunction();

		if (multiplexer == nullptr) {
			cout << __PRETTY_FUNCTION__ << " : " << "multiplexer == nullptr" << endl;
			exit(1);
		}

		if (o.mode == "training") {
			setMode(Mode::Training);
			cout << "Training Mode" << endl;
		}
		else if (o.mode == "prediction") {
			setMode(Mode::Prediction);
			cout << "Prediction Mode" << endl;
		}
		else{
			std::cerr << "Unknown Mode" << std::endl;
		}

		m_visualize = o.visualize;
		m_predictorName = o.predictorName;
		m_classifierName = o.classifierName;
		m_outputName = o.outputName;
		m_numOfClasses = o.numOfClasses;

		const std::string outputCSVFilename = m_outputName + ".csv";
		ofstream m_outputFile{outputCSVFilename};
		m_outputFile.close();

		switch (m_mode) {
			case Mode::Training:
				m_predictor.init();	
				break;
			case Mode::Prediction:
				const std::string datPredictorFile = m_predictorName + ".dat";
				const std::string datClassifierFile = m_classifierName + ".dat";

				m_predictor.load(datPredictorFile);
				cout << "Prediction: Tree was successfully loaded " << endl;
				m_classifier.load(datClassifierFile);
//				m_classifier.printTree();
				cout << "Classifier: Tree was successfully loaded " << endl;
//				m_predictor.printTree();
				break;
		}
		
		string interface = o.interface;
//		string implementation = o.implementation;

		RETURN_ON_EXCEPTION( m_uidAttrID = facade->lookup_attribute_by_name("program", "description/user-id").aID; );
		RETURN_ON_EXCEPTION( m_fnAttrID  = facade->lookup_attribute_by_name(interface, "descriptor/filename").aID; );
		RETURN_ON_EXCEPTION( m_fpAttrID  = facade->lookup_attribute_by_name(interface, "file/position").aID; );
		RETURN_ON_EXCEPTION( m_btrAttrID = facade->lookup_attribute_by_name(interface, "quantity/BytesToRead").aID; );
		RETURN_ON_EXCEPTION( m_btwAttrID = facade->lookup_attribute_by_name(interface, "quantity/BytesToWrite").aID; );
		RETURN_ON_EXCEPTION( m_brAttrID  = facade->lookup_attribute_by_name(interface, "quantity/BytesRead").aID; );
		RETURN_ON_EXCEPTION( m_bwAttrID  = facade->lookup_attribute_by_name(interface, "quantity/BytesWritten").aID; );
	}



	/**
	 * @brief Call notify function pointed by notify function pointer member.
	 * -> notifyPredictionInit -> notifyPrediction
	 * -> notifyTrainingInit -> notifyTraining
	 *
	 * @param activity
	 * @param lost
	 */
	void KnowledgeExtractor::notify(const shared_ptr<Activity>& activity, int lost) {
		(*this.*m_notifyFunction)(activity, lost);
	}



	/**
	 * @brief For sample calculation two activities are needed. This function store an activity without
	 * doing any calculation and setup another notify function that uses this activity and the next
	 * activity for sample calculation.
	 *
	 * @param activity
	 * @param lost
	 */
	void KnowledgeExtractor::notifyPredictionInit(const shared_ptr<Activity>& activity, int lost) {
		m_prevActivity = activity;
		m_notifyFunction = &KnowledgeExtractor::notifyPrediction;
	}



	/**
	 * @brief
	 *
	 * @param activity
	 * @param lost
	 */
	void KnowledgeExtractor::notifyPrediction(const shared_ptr<Activity>& activity, int lost) {
		using namespace std;
		using namespace ml::core;
		Predictor::Sample sample = constructSample(activity);
		std::vector<FeatureType> featureVector = sample.first;
		LabelType realValue = sample.second;
		LabelType predictedValue = m_predictor.predict(sample.first);
		Classifier::L cat = m_classifier.predict(sample.first);

		m_labels.push_back(realValue);
		m_predicted_labels.push_back(predictedValue);
		m_predicted_categories.push_back(cat);

		const std::string outputCSVFilename = m_outputName + ".csv";
		std::fstream m_outputFile{outputCSVFilename, std::fstream::app};

		m_outputFile << fixed << setprecision(2);
		for (const FeatureType f : featureVector) {	
			m_outputFile << f << "; ";
		}

		m_outputFile << realValue << "; " << predictedValue << "; " << cat << endl;
		m_overallTime += realValue;
		m_overallPredictedTime += predictedValue;
		size_t id = activity->attributeArray()[0].id;
		m_accessCounter[id]++;
		m_classStats[cat].counter++;
		m_classStats[cat].sumReal += realValue;
		m_classStats[cat].sumPredicted += predictedValue;

		// create anomaly
		LabelType absDiff = (realValue > predictedValue) ? (realValue - predictedValue) : (predictedValue - realValue);
		if ((double) (absDiff) / (double) (realValue) > 0.5) {
			++m_anomalyCounter;
//			cout << "found Anomaly " << m_anomalyCounter << endl;

			cout << m_anomalyCounter << " : " << absDiff << " : ";
			cout << fixed << setprecision(2);
			for (const FeatureType f : featureVector) {	
				cout << f << "; ";
			}

			cout << setprecision(6) << realValue << "; " << predictedValue << setprecision(0) << "; " << cat << endl;
		}
	}




	/**
	 * @brief For sample calculation two activities are needed. This function store an activity without
	 * doing any calculation and setup another notify function that uses this activity and the next
	 * activity for sample calculation.
	 *
	 * @param activity
	 * @param lost
	 */
	void KnowledgeExtractor::notifyTrainingInit(const shared_ptr<Activity>& activity, int lost) {
		m_prevActivity = activity;
		m_notifyFunction = &KnowledgeExtractor::notifyTraining;
	}

	
	
	/**
	 * @brief Construct a sample from activity. Put this sample in training set.
	 *
	 * @param activity
	 * @param lost
	 */
	void KnowledgeExtractor::notifyTraining(const shared_ptr<Activity>& activity, int lost) {
		using namespace std;
		using namespace ml::core;
		Predictor::Sample sample = constructSample(activity);
		m_features.push_back(sample.first);
		m_labels.push_back(sample.second);	
	}



	/**
	 * @brief 
	 *
	 * @param activity
	 * @param oaid
	 *
	 * @return 
	 */
	const Attribute* KnowledgeExtractor::findAttributeByID(const shared_ptr<Activity> & activity, OntologyAttributeID oaid) {
		//	OUTPUT( "Looking for attribute " << oaid << " in " << activity->aid() );

		const vector<Attribute> & attributes = activity->attributeArray();
		// OUTPUT( "Found att array of size " << attributes.size() );

		for(auto itr=attributes.begin(); itr != attributes.end(); itr++) {
			// OUTPUT( "...id: " << itr->id );
			if( itr->id == oaid )
				return &( *itr );
		}
		return NULL;
	}



	/**
	 * @brief 
	 *
	 * @param activity
	 *
	 * @return 
	 */
	const ActivityID* KnowledgeExtractor::findParentAID(const shared_ptr<Activity>& activity) {
		const vector<ActivityID>& parents = activity->parentArray();

		for( size_t i = parents.size(); i--; ) {
			//		if( openFileSurveys.count( parents[i] ) > 0 )
			//			return ( & parents[i] ); // Found it!
		}

		return NULL;
	}



	/**
	 * @brief 
	 */
	void KnowledgeExtractor::trainPredictor() {
		assert(m_features.size() > 0);
		assert(m_labels.size() > 0);
		assert(m_labels.size() == m_features.size());
		m_predictor.train(m_features, m_labels);
		if (m_visualize) {
			const std::string pngFile = m_predictorName + ".png";
			const std::string dotFile = m_predictorName + ".dot";
			m_predictor.exportDot(dotFile);
			int processorAvailable = system(nullptr);
			if (!processorAvailable) {
				exit(EXIT_FAILURE);
			}
			else {
				std::stringstream command;
				command << "dot -Tpng -o " << pngFile << " " << dotFile;
				std::cout << "generate graph: " << command.str() << std::endl;
				system(command.str().c_str());
			}
		}
	}



	/**
			 * @brief 
	 */
	void KnowledgeExtractor::trainClassifier() {
		assert(m_features.size() > 0);
		assert(m_labels.size() > 0);
		assert(m_labels.size() == m_features.size());
		m_classifier.train(m_features, m_numOfClasses);
		if (m_visualize) {
			const std::string pngFile = m_classifierName + ".png";
			const std::string dotFile = m_classifierName + ".dot";
			m_classifier.exportDot(dotFile);
			int processorAvailable = system(nullptr);
			if (!processorAvailable) {
				exit(EXIT_FAILURE);
			}
			else {
				std::stringstream command;
				command << "dot -Tpng -o " << pngFile << " " << dotFile;
				std::cout << "generate graph: " << command.str() << std::endl;
				system(command.str().c_str());
			}
		}
	}



	/**
	 * @brief 
	 */
	void KnowledgeExtractor::finalize() {
		using namespace std;
		for (const auto& call : m_system_call_counter) {
			const UniqueInterfaceID ucaid = m_sysinfo->lookup_activityID(m_uiid, call.first);
			cout << call.first << " : " << call.second << " : ucaid " << ucaid << endl;
		}

		multiplexer->unregisterCatchall(this, false);	
		ActivityMultiplexerPlugin::finalize();

		switch (m_mode) {
			case Mode::Training:
				{
					trainPredictor();
					const std::string datPredictorFile = m_predictorName + ".dat";
					m_predictor.save(datPredictorFile);
					trainClassifier();
					const std::string datClassifierFile = m_classifierName + ".dat";
					m_classifier.save(datClassifierFile);
				}
				break;
			case Mode::Prediction:
				cout << "overall time: " << m_overallTime << endl;
				cout << "overall predicted time:: " << m_overallPredictedTime << endl;
				assert(m_labels.size() == m_predicted_labels.size());
				assert(m_labels.size() != 0);
				assert(m_predicted_labels.size() != 0);
//				cout << fixed << setprecision(2) << "accuracy: " << m_predictor.evaluate(m_labels, m_predicted_labels) << endl;
				break;
		}

		multiplexer->unregisterCatchall( this, false );
		ActivityMultiplexerPlugin::finalize();
	}



	/**
	 * @brief 
	 *
	 * @return 
	 */
	ComponentOptions* KnowledgeExtractor::AvailableOptions() {
		return new KnowledgeExtractorOptions();
	}

	ComponentReport KnowledgeExtractor::prepareReport()
	{
		ComponentReport report{};
		uint8_t priority = 2;

		GroupEntry* geInfo = new GroupEntry{"info"};
		GroupEntry* geTime = new GroupEntry{"time", geInfo};
		report.addEntry(new GroupEntry{"real", geTime}, ReportEntry{ReportEntry::Type::APPLICATION_INFO, m_overallTime});
		report.addEntry(new GroupEntry{"predicted", geTime}, ReportEntry{ReportEntry::Type::APPLICATION_INFO, m_overallPredictedTime});

		GroupEntry* geCounter = new GroupEntry{"counter"};
		for (const auto& id : m_accessCounter) {
			OntologyAttributeFull attr = facade->lookup_attribute_by_ID(id.first);
			GroupEntry* geName = new GroupEntry{attr.name, geCounter};
			report.addEntry(new GroupEntry{"id", geName}, ReportEntry{ReportEntry::Type::APPLICATION_INFO, id.first});
			report.addEntry(new GroupEntry{"number", geName}, ReportEntry{ReportEntry::Type::APPLICATION_INFO, id.second});
		}

		GroupEntry* geClass = new GroupEntry{"class"};
		report.addEntry(new GroupEntry{"number", geClass}, ReportEntry{ReportEntry::Type::APPLICATION_PERFORMANCE, m_classStats.size()});
		for (const auto& cat : m_classStats) {
			const std::string className{"n"};
			GroupEntry* geClassNumber = new GroupEntry{std::to_string(cat.first), geClass};
			report.addEntry(new GroupEntry{"counter", geClassNumber}, ReportEntry{ReportEntry::Type::APPLICATION_PERFORMANCE, cat.second.counter, priority});
			GroupEntry* geThroughput = new GroupEntry{"throughput", geClassNumber};
			report.addEntry(new GroupEntry{"real", geThroughput}, ReportEntry{ReportEntry::Type::APPLICATION_PERFORMANCE, cat.second.sumReal / cat.second.counter, priority});
			report.addEntry(new GroupEntry{"predicted", geThroughput}, ReportEntry{ReportEntry::Type::APPLICATION_PERFORMANCE, cat.second.sumPredicted / cat.second.counter, priority});
			++priority;
		}
		return report;
	}

	extern "C" {
		void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
		{
			return new KnowledgeExtractor();	
		}
	}
}		/* -----  end of namespace kep  ----- */
