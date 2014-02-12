#include "php_siox.h"
#include "siox_engine.h"

zend_object_handlers siox_engine_object_handlers;

struct siox_engine_object {
	zend_object std;
	SIOX_Engine *siox_engine;
};

zend_class_entry *siox_engine_ce;

void siox_engine_free_storage(void *object TSRMLS_DC)
{
	siox_engine_object *obj = (siox_engine_object *) object;
	delete obj->siox_engine;
	
	zend_hash_destroy(obj->std.properties);
	FREE_HASHTABLE(obj->std.properties);
	
	efree(obj);
}

zend_object_value siox_engine_create_handler(zend_class_entry *type TSRMLS_DC)
{
	zval *tmp;
	zend_object_value retval;
	
	siox_engine_object *obj = (siox_engine_object *) emalloc(sizeof(siox_engine_object));
	memset(obj, 0, sizeof(siox_engine_object));
	obj->std.ce = type;
	
	ALLOC_HASHTABLE(obj->std.properties);
	zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(obj->std.properties, &type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
//	retval.handle = zend_objects_store_put(obj, NULL, siox_engine_free_storage, NULL, TSRMLS_CC);
	retval.handle = zend_objects_store_put(obj, NULL, siox_engine_free_storage, NULL);
	retval.handlers = &siox_engine_object_handlers;
	
	return retval;
}

PHP_METHOD(SIOX_Engine, __construct)
{
	SIOX_Engine *siox_engine = NULL;
	zval *object = getThis();
	
	siox_engine = new SIOX_Engine();
	siox_engine_object *obj = (siox_engine_object *) zend_object_store_get_object(object TSRMLS_CC);
	obj->siox_engine = siox_engine;
}

PHP_METHOD(SIOX_Engine, test)
{
	SIOX_Engine *siox_engine;
	siox_engine_object *obj = (siox_engine_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	siox_engine = obj->siox_engine;
	if (siox_engine != NULL) {
		siox_engine->test();
	}
}

function_entry siox_engine_methods[] = {
	PHP_ME(SIOX_Engine, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(SIOX_Engine, test,        NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(siox)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "SIOX_Engine", siox_engine_methods);
// 	siox_engine_ce = zend_register_internal_class(&ce, TSRMLS_CC);
	siox_engine_ce = zend_register_internal_class(&ce);
	siox_engine_ce->create_object = siox_engine_create_handler;
	memcpy(&siox_engine_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	siox_engine_object_handlers.clone_obj = NULL;
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
