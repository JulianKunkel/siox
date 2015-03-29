#ifndef TOOLS_TRACEREADER_PLUGIN_H
#define TOOLS_TRACEREADER_PLUGIN_H

#include <assert.h>

#include <unordered_map>
#include <string>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include <core/component/Component.hpp>
#include <tools/TraceReader/TraceReader.hpp>


using namespace boost;

using namespace std;
using namespace ::core;

#define DEBUG( ... ) do { if ( this->verbosity > 3 ) cerr << __VA_ARGS__ << endl; } while(0);

namespace tools {
	class TraceReaderPlugin;

	struct TraceReaderOptions: public ComponentOptions{
		TraceReader * traceReader;
		program_options::variables_map * vm;
		int verbosity;
		TraceReaderPlugin * nextPlugin;
	};

	class TraceReaderPlugin: public Component {
	protected:
		TraceReader * tr;
		program_options::variables_map * vm;
		int verbosity;

		virtual void init(program_options::variables_map * vm, TraceReader * tr) = 0;

		TraceReaderPlugin * nextInChain = nullptr;

		void forwardActivity(std::shared_ptr<Activity> a){
			if ( nextInChain != nullptr ){
				nextInChain->nextActivity(a);
			}
		}

		virtual std::shared_ptr<Activity> processNextActivity(std::shared_ptr<Activity> activity) = 0;		
	public:

		ComponentOptions * AvailableOptions() override{
			return new TraceReaderOptions();
		}

		void init() override {
			auto o = getOptions<TraceReaderOptions>();
			this->tr = o.traceReader;
			this->vm = o.vm;
			this->verbosity = o.verbosity;
			this->nextInChain = o.nextPlugin;

			assert( this->vm );
			assert( this->tr );

			init( this->vm, this->tr );
		}

		virtual void finalize(){};

		void nextActivity(std::shared_ptr<Activity> activity){
			std::shared_ptr<Activity> a = processNextActivity(activity);
//			if ( a != activity ){ 
//				// the original activity is not thrown again
//				delete(activity);
//			}
			if( a != nullptr ){
				forwardActivity ( a ); 
			}
		}


		virtual void moduleOptions(program_options::options_description & od) = 0;
	};

}

#define TRACE_READER_PLUGIN_INTERFACE "tools_traceReader_plugin"

#endif

