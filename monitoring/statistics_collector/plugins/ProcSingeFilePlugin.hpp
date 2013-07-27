#include <boost/regex.hpp>
#include <fstream>
#include <vector>
#include <string> 

#include <monitoring/statistics_collector/StatisticsProviderPluginImplementation.hpp>
#include <core/component/ComponentReferenceSerializable.hpp>

using namespace std;
using namespace monitoring;
using namespace core;
using namespace boost;


template<int MAX_NUM>
class ProcSingeFilePlugin: public StatisticsProviderPlugin{
private:

	template<int INIT>
	void parseProcLine(const string & filename){
		regex re("[ \t]*[^ \t]+");

		ifstream file(filename);
		if(! file.good()){
			// TODO add error value.
			return;
		}
		int lineNr = 0;
		while(! file.eof()){
			string line;
			getline (file, line);


			if(line.size() < 8)
				continue;

			regex_iterator<string::iterator> rit ( line.begin(), line.end(), re );
			regex_iterator<string::iterator> rend;

			vector<string> splittedArray;
			int i=0;
			while (rit!=rend && i < MAX_NUM) {
				// trim leading spaces
				const string & match  = rit->str();
				size_t startpos = match.find_first_not_of(" \t");
				if( string::npos != startpos )
				{
			    	splittedArray.push_back(match.substr( startpos ));
				}else{			
						splittedArray.push_back(match);
				}
				
				i++;
			  	++rit;
			}
			lineNr++;
			if(INIT){
				initLine(lineNr, splittedArray);
			}else{
				timestepLine(lineNr, splittedArray);
			}
		}

		file.close();
	}

protected:
	virtual void timestepLine(int lineNr, vector<string> & entries) = 0;	
	virtual const string filename() = 0;
	
	//virtual void init(int linecount);
	// During the initalization this function is called.
	virtual void initLine(int lineNr, vector<string> & entries) = 0;
public:

	void init(){
		parseProcLine<1>(filename());
	}

	virtual void nextTimestep(){
		parseProcLine<0>(filename());
	}
};


