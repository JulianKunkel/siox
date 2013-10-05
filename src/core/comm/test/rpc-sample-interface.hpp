#ifndef RPC_SAMPLE_INTERFACE_HPP_
#define RPC_SAMPLE_INTERFACE_HPP_

#include <string.h>
#include <core/component/Component.hpp>

using namespace std;

//@serializable
struct DataForRPC1{
	int64_t a;
};

//@serializable
struct DataForRPC2{
	string b;
};

//@serializable
struct DataFromRPC2{
	int32_t c;
	string d;
};


class SampleInterface : public core::Component{
public:
	virtual void rpc1(const DataForRPC1 & data) = 0;
	virtual DataFromRPC2 rpc2(const DataForRPC2 & data) = 0;

	virtual ~SampleInterface(){};
};

#define RPCSAMPLE_CLIENT_INTERFACE "rpc_sample_client"
#define RPCSAMPLE_SERVER_INTERFACE "rpc_sample_server"

// usually this definition is provided in the implementation file, but here we simplify things
#define RPCSAMPLE_INSTANCIATOR_NAME MODULE_INSTANCIATOR_NAME(rpc_sample_client)
#define RPCSAMPLE_SERVER_INSTANCIATOR_NAME MODULE_INSTANCIATOR_NAME(rpc_sample_server)


#endif