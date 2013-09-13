#include "../../../test/test-comm-interfaces.hpp"

int main(){
	runTestSuite("siox-core-comm-gio", "localhost:8081", "localhost:8082", "localhost:8083");

	return 0;
}

