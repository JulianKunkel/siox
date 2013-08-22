#ifndef CORE_MATHEMATICALEQUATION_HPP
#define CORE_MATHEMATICALEQUATION_HPP

#include <string>
#include <vector>
#include <utility>
#include <core/datatypes/VariableDatatype.hpp>

using namespace std;

namespace core {
	/*
	 * This class parses a mathematical equation. Variables can be provided and evaluated to the corresponding numerical value.
	 */
	class MathematicalEquation {
		public:
			enum MathValueType {
				INT64, UINT64, INT32, UINT32, FLOAT, DOUBLE, LONG_DOUBLE
			};

// precision(double) 16 significant digits
// for computation precision(long double) 19 significant digits
// Value of type double to value of type double can be computed using long double

			/*
			 Evaluate the equation
			 */
			double evaluate();


			static double evalSingleExpression( string const & equation ) {
				return MathematicalEquation( equation ).evaluate();
			}


			static double evalSingleExpression( string const & equation, const vector<double> & values );


			MathematicalEquation() {
				equation = "0";

				init();
			}

			MathematicalEquation( string const & equation )  : equation( equation )  {
				init();
			}

			MathematicalEquation( string const & equation, const vector<pair<MathValueType, void *> > & variables, bool is_value )  : equation( equation ), variables( move( variables ) ), is_pointer( is_value ) {
				init();
			}

			MathematicalEquation( string const & equation, const vector<pair<MathValueType, void *> > & variables )  : equation( equation ), variables( move( variables ) ) {
				init();
			}

			friend double getValue( int which, MathematicalEquation & e );



		private:
			void init();
			string equation;

			vector<pair<MathValueType, void *> > variables;

			// if is_pointer = false then we expect all variables to be doubles.
			bool is_pointer = true;

	};

}

#endif