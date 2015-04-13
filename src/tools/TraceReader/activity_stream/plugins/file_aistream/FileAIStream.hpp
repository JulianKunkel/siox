#ifndef  FileAIStream_INC
#define  FileAIStream_INC

#include <string>
#include <core/module/ModuleLoader.hpp>
#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivitySerializationPlugin.hpp>
#include <tools/TraceReader/activity_stream/ActivityInputStreamPlugin.hpp>
#include <tools/TraceReader/activity_stream/ActivityInputStreamPluginImplementation.hpp>
#include "FileAIStreamOptions.hpp"

namespace tools {

	class FileAIStream : public tools::ActivityInputStreamPlugin {
		public:
			FileAIStream();
			virtual ~FileAIStream();
			std::shared_ptr<monitoring::Activity> nextActivity() override;
			core::ComponentOptions* AvailableOptions() override {return new FileAIStreamOptions{};}
			void init() override;

		private:
			monitoring::ActivitySerializationPlugin* m_activity_deserializer = nullptr;
			std::string m_filename;
	};

}		/* -----  end of namespace tools  ----- */
#endif   /* ----- #ifndef FileAIStream_INC  ----- */
