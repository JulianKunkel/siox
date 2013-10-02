#include "../../../test/test-comm-interfaces.hpp"

int main(){
	runTestSuite("siox-core-comm-inmemory", "local:1", "local:2", "local:3");
	
	return 0;
}