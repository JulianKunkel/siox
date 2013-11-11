#ifndef UTIL_REPORTER_HELPER_HPP
#define UTIL_REPORTER_HELPER_HPP

#include <core/component/ComponentRegistrar.hpp>

namespace util {

/*
 This function gathers all reports and sends them to available reporters.
 */
void invokeAllReporters(core::ComponentRegistrar * registrar);

}

#endif