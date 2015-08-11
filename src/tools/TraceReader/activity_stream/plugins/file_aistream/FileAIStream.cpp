#include <assert.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include <core/module/ModuleLoader.hpp>
#include <util/autoLoadModules.hpp>

#include "FileAIStream.hpp"


namespace tools {

	FileAIStream::FileAIStream() {
		assert( m_activity_deserializer == nullptr );
		// TODO: when ActivitySerializationPlugin is auto loadable:
		// - Create entry in siox.conf
		// - Make ActivitySerializationPlugin auto loadable
		// - Link this plugin to ActivitySerialiationPlugin
		m_activity_deserializer = core::module_create_instance<monitoring::ActivitySerializationPlugin>(
				"", 
				"siox-monitoring-activityPlugin-ActivityBinWriter", 
				ACTIVITY_SERIALIZATION_PLUGIN_INTERFACE );
		assert( m_activity_deserializer != nullptr );
	} // constructor 



	FileAIStream::~FileAIStream(){
		if (m_activity_deserializer) {
			m_activity_deserializer->closeTrace();
			delete(m_activity_deserializer);
			m_activity_deserializer = nullptr;
		}
	} // destructor



	void FileAIStream::init() {
		FileAIStreamOptions& options = getOptions<FileAIStreamOptions>();
		std::string filename = options.filename;	
		out = GET_INSTANCE(ActivityMultiplexer, options.target_multiplexer);
		m_activity_deserializer->loadTrace(filename);
	}



	std::shared_ptr<monitoring::Activity> FileAIStream::nextActivity() {
		try{
			if(m_activity_deserializer->hasNextActivity()) {
				return  m_activity_deserializer->nextActivity();
			}
			else {
				return  std::shared_ptr<monitoring::Activity>{nullptr};
			}
		}catch(exception& e){
			cerr << "Error while reading trace entry: " << e.what() << endl;
			exit(1);
			// return std::shared_ptr<Activity>{nullptr}; // probably not a good idea
		}
		return  std::shared_ptr<monitoring::Activity>{nullptr};
	}

	monitoring::ActivityMultiplexer* FileAIStream::getTargetMultiplexer() {
		return out;
	}

	extern "C" {
		void* TOOLS_ACTIVITY_INPUT_STREAM_PLUGIN_INSTANCIATOR_NAME ()
		{
			return new FileAIStream();
		}
	}

}		/* -----  end of namespace tools  ----- */
