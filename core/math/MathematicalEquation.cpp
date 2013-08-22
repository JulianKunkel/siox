/*
 * This class is heavily based on Eval.tree.hpp  * File:   MathematicalEquation.tree.hpp  * Author: Lukasz Czerwinski  * Website: www.lukaszczerwinski.pl
 *
 */

#include <vector>
#include <utility>
#include <iostream>
#include <stdexcept>

#include <exception>
#include <cmath>

#include <boost/lexical_cast.hpp>

#include <boost/spirit/include/classic_attribute.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_parse_tree.hpp>
#include <boost/spirit/include/classic_ast.hpp>

#include <boost/spirit/include/phoenix1.hpp>

#include <core/math/MathematicalEquation.hpp>

using namespace core;
using namespace std;
namespace spirit_cls = BOOST_SPIRIT_CLASSIC_NS;


namespace core {


	typedef spirit_cls::tree_match< char const * >::const_tree_iterator iterator;


	static spirit_cls::rule <
	spirit_cls::scanner <
	char const *,
	     spirit_cls::scanner_policies <
	     spirit_cls::iteration_policy,
	     spirit_cls::pt_match_policy< char const * >,
	     spirit_cls::action_policy
	     >
	     > > condition_expression, or_expression, and_expression, equal_expression, comparision_expression, expression, term, power, factor, function_call, number, index;

	static bool initalized = false;
	class MathematicalEquation;

	static double evaluate_condition_expression( iterator const & i, MathematicalEquation & e );

	void MathematicalEquation::init()
	{
		if( initalized ) {
			return;
		}
		initalized = true;

		condition_expression
		= or_expression
		  >> !( '?' >> or_expression >> ':' >> or_expression )
		  ;

		or_expression
		= and_expression
		  >> *( "||" >> and_expression )
		  ;

		and_expression
		= equal_expression
		  >> *( "&&" >> equal_expression )
		  ;

		equal_expression
		= comparision_expression
		  >> *( ( "==" >> comparision_expression )
		        | ( "!=" >> comparision_expression )
		      )
		  ;

		comparision_expression
		= expression
		  >> *( ( "<=" >> expression )
		        | ( "<" >> expression )
		        | ( ">=" >> expression )
		        | ( ">" >> expression )
		      )
		  ;

		expression
		=   term
		    >> *( ( '+' >> term )
		          | ( '-' >> term )
		        )
		    ;

		term
		=   power
		    >> *( ( '*' >> power )
		          | ( '/' >> power )
		        )
		    ;

		power
		=   factor >> !( spirit_cls::root_node_d[ spirit_cls::ch_p( '^' ) ] >> power )
		    ;

		factor
		=   number
		    | ( '(' >> condition_expression >> ')' )
		    | ( '-' >> factor )
		    | ( '+' >> factor )
		    | ( '$' >> index )
		    |   function_call
		    ;

		function_call
		= spirit_cls::token_node_d[ +spirit_cls::alpha_p ] >> '(' >> condition_expression >> ')'
		  ;

		number
		= spirit_cls::real_p
		  ;

		index
		= spirit_cls::uint_parser< int, 10, 1, 1>()
		  ;
	}

	double MathematicalEquation::evaluate()
	{
		spirit_cls::tree_parse_info<> info
		= spirit_cls::pt_parse(
		      equation.c_str(),
		      condition_expression
		  );

		if( ! info.full )
			throw std::runtime_error(
			    std::string( "Parsing failed. Stopped at: " ) + info.stop
			);

		return evaluate_condition_expression( info.trees.begin(), *this );
	}


	static inline double evaluate_number( iterator const & i )
	{
		return boost::lexical_cast< double >( std::string( i->value.begin(), i->value.end() ) );
	}

	static inline size_t evaluate_index( iterator const & i )
	{
		return boost::lexical_cast< size_t >( std::string( i->value.begin(), i->value.end() ) );
	}

	static double evaluate_function_call( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == function_call.id() );

		iterator current = i->children.begin();

		std::string const functionName( current->value.begin(), current->value.end() );
		++current; // skip function name

		assert( * ( current->value.begin() ) == '(' );
		++current; // skip '('

		double const expressionValue = evaluate_condition_expression( current, e );
		++current; // skip expression

		double result = 0.0;

		if( functionName == "abs" )
			result = abs( expressionValue );
		else if( functionName == "sin" )
			result = sin( expressionValue );
		else if( functionName == "asin" )
			result = asin( expressionValue );
		else if( functionName == "cos" )
			result = cos( expressionValue );
		else if( functionName == "acos" )
			result = acos( expressionValue );
		else if( functionName == "rad" )
			result = 3.141592 * expressionValue / 180;
		else if( functionName == "deg" )
			result = 180 * expressionValue / 3.141592;
		else if( functionName == "ln" )
			result = log( expressionValue );
		else if( functionName == "ln10" )
			result = log10( expressionValue );


		assert( * ( current->value.begin() ) == ')' );
		++current;

		return result;
	}

	inline double getValue( int which, MathematicalEquation & e )
	{
		pair<MathematicalEquation::MathValueType, void *> & var = e.variables[which];

		if( ! e.is_pointer ) {
			double r;
			memcpy( & r, & var.second, sizeof( double ) );
			return r;
		}

		switch( var.first ) {
			case MathematicalEquation::INT32:
				return  *( ( int32_t * ) var.second );
			case MathematicalEquation::INT64:
				return  *( ( int64_t * ) var.second );
			case MathematicalEquation::UINT32:
				return  *( ( uint32_t * ) var.second );
			case MathematicalEquation::UINT64:
				return  *( ( uint64_t * ) var.second );
			case MathematicalEquation::FLOAT:
				return  *( ( float * ) var.second );
			case MathematicalEquation::DOUBLE:
				return  *( ( double * ) var.second );
			default:
				assert( false );
		}
	}

	static double evaluate_factor( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id id = i->value.id();
		assert( id == factor.id() );

		iterator current = i->children.begin();
		id = current->value.id();

		if( id == number.id() ) {
			return evaluate_number( current->children.begin() );
		} else if( id == function_call.id() ) {
			return evaluate_function_call( current, e );
		} else if( * ( current->value.begin() ) == '(' ) {
			++ current;
			return evaluate_condition_expression( current, e );
		} else if( * ( current->value.begin() ) == '-' ) {
			++ current;
			return -evaluate_factor( current, e );
		} else if( * ( current->value.begin() ) == '+' ) {
			++ current;
			return evaluate_factor( current, e );
		} else if( * ( current->value.begin() ) == '$' ) {
			++ current;
			// TODO fixme, without a loop it is not possible to know the variants type here...
			return  getValue( evaluate_index( ( current )->children.begin() ), e );
		} else {
			assert( ! "evaluate_factor" );
			return 0.0;
		}
	}

	static double evaluate_power( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == power.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double leftPowerResult = evaluate_factor( current, e );

		for( ++ current ; current != end ; ++current ) {
			char const operatorChar = * ( current->value.begin() );

			++ current;
			double const rightPowerResult = evaluate_power( current, e );

			if( operatorChar == '^' )
				leftPowerResult = std::pow( leftPowerResult, rightPowerResult );
			else
				assert( 0 );
		}
		return leftPowerResult;
	}

	static double evaluate_term( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == term.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double leftFactorResult = evaluate_power( current, e );

		for( ++ current ; current != end ; ++current ) {
			char const operatorChar = *( current->value.begin() );

			++ current;
			double const rightFactorResult = evaluate_power( current, e );

			if( operatorChar == '*' )
				leftFactorResult *= rightFactorResult;
			else if( operatorChar == '/' )
				leftFactorResult /= rightFactorResult;
			else
				assert( 0 );
		}
		return leftFactorResult;
	}

	static double evaluate_expression( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == expression.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double leftTermResult = evaluate_term( current, e );

		for( ++ current ; current != end ; ++current ) {
			char const operatorChar = * ( current->value.begin() );

			++ current;
			double const rightTermResult = evaluate_term( current, e );

			if( operatorChar == '+' )
				leftTermResult += rightTermResult;
			else if( operatorChar == '-' )
				leftTermResult -= rightTermResult;
			else
				assert( 0 );
		}
		return leftTermResult;
	}

	static double evaluate_comparision_expression( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == comparision_expression.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double leftExpressionResult = evaluate_expression( current, e );

		for( ++ current ; current != end ; ++current ) {
			std::string const operatorString = std::string( current->value.begin(), current->value.end() );

			++ current;
			double const rightExpressionResult = evaluate_expression( current, e );

			if( operatorString == ">=" )
				leftExpressionResult =  leftExpressionResult >= rightExpressionResult;
			else if( operatorString == ">" )
				leftExpressionResult =  leftExpressionResult > rightExpressionResult;
			else if( operatorString == "<" )
				leftExpressionResult =  leftExpressionResult < rightExpressionResult;
			else if( operatorString == "<=" )
				leftExpressionResult =  leftExpressionResult <= rightExpressionResult;
			else
				assert( 0 );
		}

		return leftExpressionResult;
	}

	static double evaluate_equal_expression( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == equal_expression.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double leftExpressionResult = evaluate_comparision_expression( current, e );

		for( ++ current ; current != end ; ++current ) {
			std::string const operatorString = std::string( current->value.begin(), current->value.end() );

			++ current;
			double const rightExpressionResult = evaluate_comparision_expression( current, e );

			if( operatorString == "==" )
				leftExpressionResult =  leftExpressionResult == rightExpressionResult;
			else if( operatorString == "!=" )
				leftExpressionResult =  leftExpressionResult != rightExpressionResult;
			else
				assert( 0 );
		}

		return leftExpressionResult;
	}

	static double evaluate_and_expression( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == and_expression.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double leftExpressionResult = evaluate_equal_expression( current, e );

		for( ++ current ; current != end ; ++current ) {
			std::string const operatorString = std::string( current->value.begin(), current->value.end() );

			++ current;
			double const rightExpressionResult = evaluate_equal_expression( current, e );

			if( operatorString != "&&" )
				assert( 0 );

			if( leftExpressionResult != 0.0 )
				leftExpressionResult = rightExpressionResult;
		}

		return leftExpressionResult;
	}

	static double evaluate_or_expression( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == or_expression.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double leftExpressionResult = evaluate_and_expression( current, e );

		for( ++ current ; current != end ; ++current ) {
			std::string const operatorString = std::string( current->value.begin(), current->value.end() );

			++ current;
			double const rightExpressionResult = evaluate_and_expression( current, e );

			if( operatorString != "||" )
				assert( 0 );

			if( leftExpressionResult == 0.0 )
				leftExpressionResult = rightExpressionResult;
		}

		return leftExpressionResult;
	}

	static double evaluate_condition_expression( iterator const & i, MathematicalEquation & e )
	{
		spirit_cls::parser_id const id = i->value.id();
		assert( id == condition_expression.id() );

		iterator current = i->children.begin();
		iterator const end = i->children.end();

		double const condition = evaluate_or_expression( current, e );

		if( ++ current != end ) {
			assert( std::string( "?" ) == std::string( current->value.begin(), current->value.end() ) );

			++ current;
			double const trueExpression = evaluate_or_expression( current, e );

			++ current;
			assert( std::string( ":" ) == std::string( current->value.begin(), current->value.end() ) );

			++ current;
			double const falseExpression = evaluate_or_expression( current, e );

			if( condition )
				return trueExpression;
			else
				return falseExpression;
		}

		return condition;
	}


	double MathematicalEquation::evalSingleExpression( std::string const & equation, const vector<double> & values )
	{
		vector<pair<MathematicalEquation::MathValueType, void *> > val( values.size() );
		assert( sizeof( double ) == sizeof( void * ) );

		for( unsigned i = 0; i < values.size(); i++ ) {
			void * v = nullptr;
			memcpy( & v, & values[i], sizeof( double ) );

			val[i] = {DOUBLE, v};
		}

		MathematicalEquation e( equation, val, false );
		double ret = e.evaluate();

		return ret;
	}


}
