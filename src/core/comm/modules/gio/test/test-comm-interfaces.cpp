#include "../../../test/test-comm-interfaces.hpp"

int main(){
	//for(int i=0; i < 50; i++){ 

	cout << endl << "Testing TCP" << endl;
	runTestSuite("siox-core-comm-gio", "localhost:8081", "tcp://localhost:8082", "localhost:8083");
	//}

	cout << endl << "Testing IPC" << endl;
	runTestSuite("siox-core-comm-gio", "ipc://alpha", "ipc://beta", "ipc://gamma");

	//cout << endl << "Testing SCTP" << endl;
	//runTestSuite("siox-core-comm-gio", "sctp://localhost:8081", "sctp://localhost:8082", "sctp://localhost:8083");

 
	return 0;
}

