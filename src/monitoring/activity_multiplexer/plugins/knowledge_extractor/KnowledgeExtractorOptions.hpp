/*
 * =====================================================================================
 *
 *       Filename:  AmuxPluginOptions.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/09/2015 04:59:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef KNOWLEDGEEXTRACTOROPTIONS_HPP
#define KNOWLEDGEEXTRACTOROPTIONS_HPP

#include <monitoring/activity_multiplexer/ActivityMultiplexerPluginOptions.hpp>
#include <string>
#include <vector>

//@serializable
class KnowledgeExtractorOptions : public monitoring::ActivityMultiplexerPluginOptions {
	public:
		std::string filename;
		std::string interface;
		std::string implementation;
		std::string mode;
		bool visualize;
		std::string predictorName;
		std::string classifierName;
		std::string outputName;
		unsigned int numOfClasses;
		std::vector<std::string> openTokens;
		std::vector<std::string> seekTokens;
		std::vector<std::string> readTokens;
		std::vector<std::string> writeTokens;
		std::vector<std::string> closeTokens;
};

#endif
