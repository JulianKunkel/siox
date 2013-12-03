/**
 * Some utility macros for dealing with exceptions in common ways.
 *
 * All these macros take a variable argument list of statements that are executed within a try block.
 * This try block is followed by a catch that matches any exception, ensuring that no exception can ever come ouf of one of these macros.
 * The macros differ in what they do when they catch an exception.
 *
 * @author Nathanael Hübbe
 * @date 2013
 */

#ifndef INCLUDE_GUARD_EXCEPTION_HANDLING_HPP
#define INCLUDE_GUARD_EXCEPTION_HANDLING_HPP

#include <stdlib.h>

///Catch any exception and proceed normally.
#define IGNORE_EXCEPTIONS(...) do { \
	try { \
		__VA_ARGS__ \
	} catch(...) { \
	} \
} while(0)

///Catch any exception and return immediately if there was one.
#define RETURN_ON_EXCEPTION(...) do { \
	try { \
		__VA_ARGS__ \
	} catch(...) { \
		return; \
	} \
} while(0)

/**
 * Print an error message if an exception is caught.
 *
 * message is usually a string, but it can be anything that can be streamed to cerr. Example:
 * MESSAGE_ON_EXCEPTION("internal problem detected (foo = " << foo << "). Please report this bug.\n", doStuff(foo););
 */
#define MESSAGE_ON_EXCEPTION(message, ...) do { \
	try { \
		__VA_ARGS__ \
	} catch(...) { \
		cerr << message; \
	} \
} while(0)

/**
 * Print an error message and abort() if an exception is caught.
 *
 * message is usually a string, but it can be anything that can be streamed to cerr. Example:
 * MESSAGE_ON_EXCEPTION("internal problem detected (foo = " << foo << "), bailing out. Please report this bug.\n", doStuff(foo););
 */
#define ABORT_ON_EXCEPTION(message, ...) do { \
	try { \
		__VA_ARGS__ \
	} catch(...) { \
		cerr << message; \
		abort(); \
	} \
} while(0)

#endif
