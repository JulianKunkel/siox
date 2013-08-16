#ifndef CONTAINER_SERIALIZER_TEXT_H
#define CONTAINER_SERIALIZER_TEXT_H

#include <iostream>
#include <fstream>
#include <string>

#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 

using namespace std;

namespace core{
template<class CONTAINER>
class FileSerializer{
private:
	ofstream file;
	boost::archive::text_oarchive archive;
public:
	FileSerializer(string filename): file(filename), archive(file, boost::archive::no_header | boost::archive::no_codecvt){}

	inline void append(CONTAINER * obj){
		archive << obj;
		// add end of line to force floshing
		//file << endl;
	}

	void close(){
		file.close();
	}

	~FileSerializer(){
		close();
	}
};


template<class CONTAINER>
class FileDeserializer{
private:
	ifstream file;
	boost::archive::text_iarchive archive;
public:
	FileDeserializer(string filename): file(filename), archive(file, boost::archive::no_header | boost::archive::no_codecvt){}

	inline CONTAINER * parseNext(){
		CONTAINER * obj;
		archive >> obj;
		// fetch end of line...
		//char c;
		//archive >> c;
		return obj;
	}

	inline bool hasNext(){
		return ! file.eof();
	}
};

}

#endif
