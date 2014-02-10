#ifndef PHP_SIOX_H
#define PHP_SIOX_H

#define PHP_SIOX_EXTNAME "siox"
#define PHP_SIOX_EXTVER  "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
}

extern zend_module_entry siox_module_entry;
#define phpext_siox_ptr &siox_module_entry;

#endif // PHP_SIOX_H
