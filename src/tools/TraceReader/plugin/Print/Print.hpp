#ifndef TOOLS_TRACE_PLOTTER_H
#define TOOLS_TRACE_PLOTTER_H

#include <regex>
#include <unordered_map>
#include <memory>

#include <tools/TraceReader/TraceReaderPluginImplementation.hpp>

using namespace tools;

class PrintPlugin: public TraceReaderPlugin{
	protected:
		void init(program_options::variables_map * vm, TraceReader * tr) override;
	public:
		std::shared_ptr<Activity> processNextActivity(std::shared_ptr<Activity> activity) override;

		void moduleOptions(program_options::options_description & od) override;
};

#endif
