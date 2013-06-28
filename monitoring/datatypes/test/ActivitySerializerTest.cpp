#include <sstream>

#include <monitoring/datatypes/ActivitySerializable.hpp>
#include <core/container/container-serializer-text.hpp>

using namespace monitoring;
using namespace core;
using namespace std;


int main(){
	ComponentID cid = {.pid = {2,3,4}, .uuid= {1,2}};

	auto * parentArray = new vector<ComponentID>{{.pid = {1,2,3}, .uuid= {2,2}}};
	auto * attributeArray = new vector<Attribute>{{.id=111, .value = "myData"}, {.id=3, . value = (uint64_t) 4711}};
	auto * remoteCallsArray = new vector<RemoteCall>();

	// Cast the real activity to the serializable object class wrapper
	ActivitySerializable * test = (ActivitySerializable*) new Activity("test", 3, 5, cid, parentArray, attributeArray, remoteCallsArray, NULL, 0);
	cout << test->name() << endl;	

	// output test to variable
	stringstream ss;
	boost::archive::text_oarchive oa(ss); 
	oa << test;
	cout << ss.str() << endl;
	
	// output to file
	FileSerializer<ActivitySerializable> outFile("data.txt");
	outFile.append(test);
	outFile.close();

	// read test from variable
	ActivitySerializable * parsedActivity;
	boost::archive::text_iarchive ia(ss); 
	ia >> parsedActivity;
	cout << parsedActivity->name() << endl;

	// read back from file
	FileDeserializer<ActivitySerializable> inFile("data.txt");
	ActivitySerializable * readActivity;
	while(inFile.hasNext()){
		readActivity = inFile.parseNext();
	}
}
