#include <assert.h>
#include <iostream>
#include <unistd.h>

#include <mutex>
#include <condition_variable>
#include <atomic>

#include <core/comm/CommunicationModule.hpp>
#include <core/module/ModuleLoader.hpp>
#include <util/TestHelper.hpp>

#include <core/container/container-binary-serializer.hpp>

using namespace core;
using namespace std;

#include <time.h>

uint64_t gettime()
{
        struct timespec tp;
        if( clock_gettime( CLOCK_REALTIME, &tp ) != 0 ) {
                return -1;
        } else {
                return ( tp.tv_sec * 1000000000ull ) + tp.tv_nsec;
        }
}

double endTimeInS(uint64_t startTime){
	return (gettime() - startTime) / 1000000000.0;
}


static int roleServer = 0;
static uint64_t messageSize = 1*1024;
static int runtime = 5;
static string address = "tcp://0.0.0.0:30000";
static bool terminated = false;
static ServiceClient * c;



class ClientConnectionCallback: public ConnectionCallback, public ProtectRaceConditions{
public:
	int retries = 0;
	bool connected = false;

	void connectionErrorCB(ServiceClient & connection, CommunicationError error){
		connected = false;
		cout << "connectionErrorCB retry: " << retries << endl;
		
		if(retries >= 2){
			cout << "Retry count reached, stopping.";
			sthHappens();
			return;
		}

		retries++;
		
		// normally we would add a sleeping time here...
		connection.ireconnect();
	}

	void connectionSuccessfullCB(ServiceClient & connection){
		connected = true;
		retries = 0;
		sthHappens();
	}
};


class PerformanceClientMessageCallback: public MessageCallback, public ProtectRaceConditions{
public:
	atomic<uint64_t> messagesSend;
	atomic<uint64_t> messageResponsesReceived;

	uint64_t messageSize;
	void * buffer = nullptr;

	PerformanceClientMessageCallback(uint64_t messageSize): messageSize(messageSize) {
		messagesSend = 0;
		messageResponsesReceived = 0;

		buffer = malloc(messageSize);
	}

	~PerformanceClientMessageCallback(){
		free(buffer);
	}

	void messageSendCB(BareMessage * msg){
		messagesSend++;

		if (! terminated){
			c->isend("");
		}
	}

	void messageResponseCB(BareMessage * msg, char * buffer, uint64_t buffer_size){
		messageResponsesReceived++;
	}

	void messageTransferErrorCB(BareMessage * msg, CommunicationError error){
		sthHappens();
	}

	uint64_t serializeMessageLen(const void * msgObject) {
		return messageSize;
	}

	void serializeMessage(const void * msgObject, char * buffer, uint64_t & pos){
		memcpy((char *) buffer + pos, this->buffer, messageSize);
		pos += messageSize;
	}


};


static void runClient(CommunicationModule * comm){
	ClientConnectionCallback ccb;
	PerformanceClientMessageCallback mcb(messageSize);

	uint64_t ct0 = gettime();

	c = comm->startClientService(address, & ccb, & mcb);
	ccb.waitUntilSthHappened();
	const double connectionTime = endTimeInS(ct0);
	cout << "Connection time: " << connectionTime << " s" << endl;

	if( ! ccb.connected ){
		cerr << "Error, could not connect to the server!" << endl;
		exit(1);
	}

	// run client performance test now

	// these are incrementing counters
	uint64_t send = 0;
	uint64_t receivedResponses = 0;

	uint64_t mstart = gettime();
	c->isend("");
	for( int t = 0; t < runtime; t++ ){
		uint64_t t0 = gettime();
		sleep(1);
		const double runTime = endTimeInS(t0);

		uint64_t inc_send = mcb.messagesSend.load();
		uint64_t inc_received = mcb.messageResponsesReceived.load();

		printf("%f s messages/s: %f responses/s: %f\n", runTime , (inc_send - send) / runTime, (inc_received - receivedResponses) / runTime );

		send = inc_send;
		receivedResponses = inc_received;
	}
	terminated = true;
	delete(c);
	send = mcb.messagesSend.load();
	receivedResponses = mcb.messageResponsesReceived.load();

	double totalRuntime = endTimeInS(mstart);
	printf("Sum: %f s messages/s: %f responses/s: %f\n", totalRuntime , send / totalRuntime, receivedResponses / totalRuntime);
	cout << "Messages send: " << send  << " outstanding responses after termination: " << (send - receivedResponses) << " == " << (double)(send - receivedResponses)/send << "%" << endl;
}



class PerformanceServerCallback: public ServerCallback, public ProtectRaceConditions{
public:
	 void messageReceivedCB(shared_ptr<ServerClientMessage> msg, const char * message_data, uint64_t buffer_size){
	 	msg->isendResponse("ok");
	 }


	void responseSendCB(ServerClientMessage * msg, BareMessage * r){

	}

	void invalidMessageReceivedCB(CommunicationError error){		

	}


	uint64_t serializeResponseMessageLen(const ServerClientMessage * msg, const void * responseType){
		return 0;
	}

	void serializeResponseMessage(const ServerClientMessage * msg,  const void * responseType, char * buffer, uint64_t & pos){
	}

};

static void runServer(CommunicationModule * comm){
	PerformanceServerCallback mySCB;
	ServiceServer * s = comm->startServerService(address, & mySCB );

	while(! terminated){
		sleep(1);
	}

	delete(s);
}



static void help(char ** argv){
		cerr << "Synopsis: " << argv[0] << " -s|-c [<address>] [Runtime [MessageSize]]" << endl; 
		cerr << "-s: server" << endl;
		cerr << "-c: client" << endl;
		cerr << "<Address> is either ipc://<NAME> or tcp://<host>:<port>" << endl;
		cerr << "Runtime and message size are only needed on the client side" << endl;
		exit(1);
}

int main(int argc, char ** argv){
	if (argc < 2) help(argv);

	if ( string(argv[1]) == "-s" ){
		roleServer = 1;
	} else if ( string(argv[1]) == "-c" ){
		roleServer = 0;
	} else{
		help(argv);
	}

	if ( argc > 2 ){
		address = argv[2];
	}

	if ( argc > 3 ){
		runtime = atoi(argv[3]);
	}
	if ( argc > 4 ){
		messageSize = atoll(argv[4]);
	}

	printf( "%s instance with messageSize: %llu runtime: %d address: %s\n", roleServer ? "Server" : "Client", (long long unsigned) messageSize, runtime, address.c_str() );


	// now start the correct role
	CommunicationModule * comm = core::module_create_instance<CommunicationModule>( "", "siox-core-comm-gio", CORE_COMM_INTERFACE );

	assert(comm != nullptr);
	comm->init();

	if (roleServer){
		runServer(comm);
	}else{
		runClient(comm);
	}

	delete(comm);
 
	return 0;
}

