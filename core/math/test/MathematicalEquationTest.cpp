#include <assert.h>
#include <iostream>
#include <cmath>
#include <stdexcept>

#include <core/math/MathematicalEquation.hpp>

using namespace std;
using namespace core;

void runTest(){
    {
        assert(MathematicalEquation::evalSingleExpression( "5" ) == 5 );
        assert(MathematicalEquation::evalSingleExpression( "5-3" ) == 2 );
        assert(MathematicalEquation::evalSingleExpression( "5-5" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "5-7" ) == -2 );  
    
        assert(MathematicalEquation::evalSingleExpression( "2+3+4" ) == 9 );
        assert(MathematicalEquation::evalSingleExpression( "2-3-4" ) == -5 );
    }std::cout << "All (1) tests: OK" << std::endl;
    {
        assert(MathematicalEquation::evalSingleExpression( "2*3" ) == 6 );
        assert(MathematicalEquation::evalSingleExpression( "4/2" ) == 2 );
        assert(MathematicalEquation::evalSingleExpression( "2*3*4" ) == 24 );
        assert(MathematicalEquation::evalSingleExpression( "16/2/4" ) == 2 );

        assert(MathematicalEquation::evalSingleExpression( "2*0" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "2*1" ) == 2 );
        assert(MathematicalEquation::evalSingleExpression( "2/2" ) == 1 );
    }std::cout << "All (2) tests: OK" << std::endl;
    {
        assert(MathematicalEquation::evalSingleExpression( "(1)" ) == 1 );
        assert(MathematicalEquation::evalSingleExpression( "(2+3)*4" ) == 20 );
        assert(MathematicalEquation::evalSingleExpression( "(2+3)*(4+5)" ) == 45 );
        
    }std::cout << "All (3) tests: OK" << std::endl;
    {
        assert(MathematicalEquation::evalSingleExpression( "$0", {0} ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "$0", {3} ) == 3 );
        assert(MathematicalEquation::evalSingleExpression( "$0+$1", {3, 4} ) == 7 );
        assert(MathematicalEquation::evalSingleExpression( "($0)", {3} ) == 3 );
        assert(MathematicalEquation::evalSingleExpression( "($0+$1)", {3, 4} ) == 7 );
    }std::cout << "All (4) tests: OK" << std::endl;
    {
        assert(MathematicalEquation::evalSingleExpression( "2^3" ) == 8 );
        assert(MathematicalEquation::evalSingleExpression( "3^4" ) == 81 );
        assert(MathematicalEquation::evalSingleExpression( "4^0.5" ) == 2 );
        assert(MathematicalEquation::evalSingleExpression( "2^3^2" ) == 512 );
    }
    std::cout << "All (5) tests: OK" << std::endl;
    {
        assert(MathematicalEquation::evalSingleExpression( "sin(0)" ) == sin(0.0) );
        assert(MathematicalEquation::evalSingleExpression( "sin(1)" ) == sin(1.0) );
        assert(MathematicalEquation::evalSingleExpression( "sin(-1)" ) == sin(-1.0) );
        assert(MathematicalEquation::evalSingleExpression( "cos(0)" ) == cos(0.0) );
        assert(MathematicalEquation::evalSingleExpression( "cos(1)" ) == cos(1.0) );
        assert(MathematicalEquation::evalSingleExpression( "cos(-1)" ) == cos(-1.0) );
    }
    std::cout << "All (6) tests: OK" << std::endl;
    {
        assert(MathematicalEquation::evalSingleExpression( "1<2" ) == 1 );
        assert(MathematicalEquation::evalSingleExpression( "2<=2" ) == 1 );
        assert(MathematicalEquation::evalSingleExpression( "1>2" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "1>=2" ) == 0 );
    }
    std::cout << "All (7) tests: OK" << std::endl;
    {
        assert(MathematicalEquation::evalSingleExpression( "2==2" ) == 1 );
        assert(MathematicalEquation::evalSingleExpression( "1!=2" ) == 1 );
        assert(MathematicalEquation::evalSingleExpression( "1==2" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "2!=2" ) == 0 );
    }
    std::cout << "All (8) tests: OK" << std::endl;
   {
        assert(MathematicalEquation::evalSingleExpression( "0&&0" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "0&&1" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "1&&0" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "1&&1" ) == 1 );

        assert(MathematicalEquation::evalSingleExpression( "0||0" ) == 0 );
        assert(MathematicalEquation::evalSingleExpression( "0||1" ) == 1 );
        assert(MathematicalEquation::evalSingleExpression( "1||0" ) == 1 );
        assert(MathematicalEquation::evalSingleExpression( "1||1" ) == 1 );
    }
    std::cout << "All (8) tests: OK" << std::endl;
    {
        try{
            assert(
               MathematicalEquation::evalSingleExpression(
                    "($0<50)?"
                        "("
                            "($0>25)?"
                                "50"
                            ":"
                                "0"
                        ")"
                    ":"
                        "("
                            "($0<75)?"
                                "75"
                            ":"
                                "100"
                        ")"
                    , {26})   == 50
            );
        }
        catch( std::runtime_error const & error ){
            std::cerr << error.what() << std::endl;
        }
    }
    std::cout << "All (9) tests: OK" << std::endl;
}


int main(int argc, char ** count){
	runTest();
}
