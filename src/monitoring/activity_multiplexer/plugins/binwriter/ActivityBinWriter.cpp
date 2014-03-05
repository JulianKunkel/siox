#include <sys/types.h>
#include <unistd.h>


#include <stdio.h>
#include <string.h>
#include <sstream>

#include <monitoring/activity_multiplexer/plugins/binwriter/ActivityBinWriter.hpp>
#include <monitoring/activity_multiplexer/plugins/binwriter/ActivityBinWriterPluginOptions.hpp>

#include <monitoring/activity_multiplexer/plugins/binwriter/ActivityBinarySerializable.hpp>

void ActivityBinWriterPlugin::Notify( const shared_ptr<Activity> & activity, int lost ) {
	if( ! file ) return;
	Activity * act = &* activity;
	uint64_t size = j_serialization::serializeLen(*act);
	char * buff = (char *) malloc(size);
	uint64_t pos = 0;
	j_serialization::serialize(* act, buff, pos);

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
	ActivityBinWriterPluginOptions & o = getOptions<ActivityBinWriterPluginOptions>();

 	stringstream buff;
 	buff<< o.filename;
 	if (! getenv("SIOX_ACTIVITY_WRITER_FILENAME_DONT_APPEND_PID")){
			buff << (long long unsigned) getpid();
	}			

	file = fopen(buff.str().c_str(), "ab");
	if (! file)	{
		printf("Error in %s %s, disabling plugin \n", __FILE__, strerror(errno));
	}
	multiplexer->registerCatchall( this, static_cast<ActivityMultiplexer::Callback>( & ActivityBinWriterPlugin::Notify ), false );
}

void ActivityBinWriterPlugin::finalize() {
	multiplexer->unregisterCatchall( this, false );
	ActivityMultiplexerPlugin::finalize();
}

ActivityBinWriterPlugin::~ActivityBinWriterPlugin() {
	if ( file != nullptr ){
		fclose(file);
	}
}

extern "C" {
	void * MONITORING_ACTIVITY_MULTIPLEXER_PLUGIN_INSTANCIATOR_NAME()
	{
		return new ActivityBinWriterPlugin();
	}
}
