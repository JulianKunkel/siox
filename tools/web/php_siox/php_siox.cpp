#include "php_siox.h"

PHP_MINIT_FUNCTION(siox)
{
	return SUCCESS;
}

zend_module_entry siox_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_SIOX_EXTNAME,
	NULL,                  /* Functions */
	PHP_MINIT(siox),
	NULL,                  /* MSHUTDOWN */
	NULL,                  /* RINIT */
	NULL,                  /* RSHUTDOWN */
	NULL,                  /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_SIOX_EXTVER,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SIOX
extern "C" {
ZEND_GET_MODULE(siox)
}
#endif


