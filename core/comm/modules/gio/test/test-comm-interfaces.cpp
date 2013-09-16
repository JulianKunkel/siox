#include "../../../test/test-comm-interfaces.hpp"

int main(){
	//for(int i=0; i < 50; i++){ 
		runTestSuite("siox-core-comm-gio", "localhost:8081", "localhost:8082", "localhost:8083");
	//}
 
	return 0;
}

