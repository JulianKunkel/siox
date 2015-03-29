#include <sys/types.h>
#include <unistd.h>


#include <stdio.h>
#include <string.h>
#include <sstream>
#include <stdexcept> 

#include <monitoring/activity_multiplexer/plugins/binwriter/ActivityBinWriter.hpp>
#include <monitoring/activity_multiplexer/plugins/binwriter/ActivityBinWriterPluginOptions.hpp>

#include <monitoring/activity_multiplexer/plugins/binwriter/ActivityBinarySerializable.hpp>
#include <monitoring/activity_multiplexer/ActivitySerializationPluginImplementation.hpp>

void ActivityBinWriterPlugin::Notify( const shared_ptr<Activity> & activity, int lost ) {
	if( ! file ) return;

	Activity * act = &* activity;
	uint64_t size = j_serialization::serializeLen(*act);
	char * buff = (char *) malloc(size);
	uint64_t pos = 0;
	j_serialization::serialize(* act, buff, pos);

	assert(pos == size);

	lock_guard<mutex> lock( notify_mutex );
	fwrite(& size, sizeof(uint64_t), 1, file);
	size_t count =  fwrite(buff, size, 1, file); //fwrite((void*) & size, sizeof(size), 1, file);

	if( count != 1){
		// error...
		printf("ERROR in %s %s\n", __FILE__, strerror(errno));
	}

	free(buff);
}

ComponentOptions * ActivityBinWriterPlugin::AvailableOptions() {
	return new ActivityBinWriterPluginOptions();
}

void ActivityBinWriterPlugin::initPlugin( ) {

}

void ActivityBinWriterPlugin::start( ) {
	ActivityBinWriterPluginOptions & o = getOptions<ActivityBinWriterPluginOptions>();

 	stringstream buff;
 	buff<< o.filename;
 	if (! getenv("SIOX_ACTIVITY_WRITER_FILENAME_DONT_APPEND_PID")){
			buff << (long long unsigned) getpid();
	}

	file = fopen(buff.str().c_str(), "ab");
	if (! file )	{
		printf("Error in %s %s, disabling plugin \n", __FILE__, strerror(errno));
	}else{
		multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( & ActivityBinWriterPlugin::Notify ), false );
	}	
	ActivityMultiplexerPlugin::start();
}

void ActivityBinWriterPlugin::stop( ) {
	if ( file != nullptr ){
		fclose(file);
		multiplexer->unregisterCatchall( this, false );
		file = nullptr;
	}	

	ActivityMultiplexerPlugin::stop();
}

void ActivityBinWriterPlugin::finalize() {
	ActivityMultiplexerPlugin::finalize();
}

ActivityBinWriterPlugin::~ActivityBinWriterPlugin() {

}

void ActivityTraceReaderPlugin::loadTrace(string configEntry){	
	if ( configEntry.size() <= 1){
		throw invalid_argument("ActivityTraceReader: No valid filename specified.");
	}	
	file = fopen(configEntry.c_str(), "rb");
	
	// TODO: setvbuf(file, buff, int mode, size_t size);

	if ( file == nullptr ){
		throw invalid_argument("ActivityTraceReader: file \"" + configEntry + "\" could not be read.");
	}		
}

void ActivityTraceReaderPlugin::closeTrace(){
	fclose(file);
}

//std::shared_ptr<Activity> ActivityTraceReaderPlugin::nextActivity(){
//	Activity* activity = new Activity{};
//	// read the next size:
//	uint64_t size;
//	int ret = fread(& size, sizeof(uint64_t), 1, file);
//	if ( ret == 0 ){
//		// TODO throw
//		return std::make_shared<Activity>{nullptr};
//	}
//	char * buffer = (char*) malloc(size);
//	assert(buffer);
//
//	ret = fread(buffer, size, 1, file);
//	if ( ret == 0 ){
//		// TODO throw
//		return std::make_shared<Activity>{nullptr};
//	}
//
//	uint64_t pos = 0;
//	j_serialization::deserialize(*activity, buffer, pos, size);
//	assert(pos == size);
//
//	free(buffer);
//	
//
//	std::shared_ptr<Activity> shared_activity = std::make_shared<Activity>{};
//	return activity;
//}
std::shared_ptr<Activity> ActivityTraceReaderPlugin::nextActivity(){
	// read the next size:
	uint64_t size;
	int ret = fread(& size, sizeof(uint64_t), 1, file);
	if ( ret == 0 ){
		// TODO throw
		std::cout << "Couldn't read file" << endl;
		return std::shared_ptr<Activity>{nullptr};
	}
	char * buffer = (char*) malloc(size);
	assert(buffer);

	ret = fread(buffer, size, 1, file);
	if ( ret == 0 ){
		// TODO throw
		std::cout << "Couldn't read file : 2" << endl;
		return std::shared_ptr<Activity>{nullptr};
	}

	uint64_t pos = 0;
	std::shared_ptr<Activity> shared_activity{new Activity()};
	j_serialization::deserialize(*shared_activity.get(), buffer, pos, size);
	assert(pos == size);

	free(buffer);
	return shared_activity;
}

bool ActivityTraceReaderPlugin::hasNextActivity(){
	return ! feof(file);
}

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityBinWriterPlugin();
	}
}

extern "C" {
	void * MONITORING_ACTIVITY_SERIALIZATION_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityTraceReaderPlugin();
	}
}

