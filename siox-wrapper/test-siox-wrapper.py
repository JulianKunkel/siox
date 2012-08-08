#!/usr/bin/env python
# -*- coding: utf-8 -*-
# encoding: utf-8
from __future__ import print_function

import unittest

sioxWrapper = __import__("siox-wrapper")


##
# @brief Unit test class for the siox-wrapper
#
# This class defines test cases for the siox-wrapper to provide some confidence
# the program works in the way it should.
#
class testParameter(unittest.TestCase):

    def setUp(self):

        self.parameter = sioxWrapper.Parameter()
        self.parameter.type = 'int'
        self.parameter.name = 'foo'

    ##
    # @brief Test the Parameter class.
    #
    # This function tests the Parameter class. This class works like a data structure
    # there shouldn't be any problems.
    def testParameter(self):

        self.assertEqual(self.parameter.type, 'int')
        self.assertEqual(self.parameter.name, 'foo')


##
# @brief Test the Function class.
#
# This class defines test cases for the Function class.
class testFunction(unittest.TestCase):

    def setUp(self):

        #Define a Function object for a function which looks like:
        # void foo ( void );
        # This function definition doesn't make much sense but is used to check
        # the right void handling.
        self.functionVoid = sioxWrapper.Function()
        parameterVoid = sioxWrapper.Parameter()

        parameterVoid.name = 'void'
        self.functionVoid.type = 'void'
        self.functionVoid.name = 'foo'
        self.functionVoid.parameters.append(parameterVoid)

        self.function = sioxWrapper.Function()
        parameter1 = sioxWrapper.Parameter()
        parameter2 = sioxWrapper.Parameter()

        self.function.name = 'bar'
        self.function.type = 'int *'

        parameter1.name = 'param1'
        parameter1.type = 'const char'
        parameter2.name = 'param2'
        parameter2.type = 'int *'

        self.function.parameters.append(parameter1)
        self.function.parameters.append(parameter2)

    ##
    # @brief Test function to check if the data definitions are correct.
    #
    # This test function only checks the data definitions. This tests should not
    # fail unless you screw up the class interface.
    def testFunction(self):

        self.assertEqual(self.functionVoid.name, 'foo')
        self.assertEqual(self.functionVoid.type, 'void')
        self.assertEqual(self.functionVoid.parameters[0].name, 'void')

        self.assertEqual(self.function.name, 'bar')
        self.assertEqual(self.function.type, 'int *')
        self.assertEqual(self.function.parameters[0].name, 'param1')
        self.assertEqual(self.function.parameters[0].type, 'const char')
        self.assertEqual(self.function.parameters[1].name, 'param2')
        self.assertEqual(self.function.parameters[1].type, 'int *')

    ##
    # @brief Test the getCall function of the Function class.
    def testGetCall(self):

        functionVoidCall = self.functionVoid.getCall()
        functionCall = self.function.getCall()

        self.assertEqual(functionVoidCall, 'foo()')
        self.assertEqual(functionCall, 'bar(param1, param2)')

    ##
    # @brief Test the get getDefintion function of the Function class.
    def testGetDefinition(self):

        functionDefinition = self.function.getDefinition()

        self.assertEqual(functionDefinition,
            'int * bar(const char param1, int * param2)')

    ##
    # @brief Test the getCallReal function of the Function class.
    def testGetCallReal(self):

        functionGetCallReal = self.function.getCallReal()
        functionVoidGetCallReal = self.functionVoid.getCallReal()

        self.assertEqual(functionVoidGetCallReal,
            '__real_foo()')
        self.assertEqual(functionGetCallReal,
            '__real_bar(param1, param2)')


    def testGetDefinitionReal(self):

        functionDefinitionReal = self.function.getDefinitionReal()

        self.assertEqual(functionDefinitionReal,
            'int * __real_bar(const char param1, int * param2)')


if __name__ == '__main__':
    unittest.main()
