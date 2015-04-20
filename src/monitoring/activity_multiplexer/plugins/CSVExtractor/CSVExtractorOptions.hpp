#ifndef CSVEXTRACTOR_HPP
#define CSVEXTRACTOR_HPP


#include <string>
#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>

//@serializable
class CSVExtractorOptions : public monitoring::ActivityMultiplexerPluginOptions {
   public:
      std::string filename;
};

#endif