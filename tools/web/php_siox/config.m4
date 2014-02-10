PHP_ARG_ENABLE(siox, 
	[whether to enable siox support],
	[  --enable-siox           Enable siox support])

if test "$PHP_SIOX" != "no"; then
	PHP_REQUIRE_CXX()
	PHP_SUBST(SIOX_SHARED_LIBADD)
	PHP_ADD_LIBRARY(stdc++, 1, SIOX_SHARED_LIBADD)
	PHP_NEW_EXTENSION(siox, php_siox.cpp siox_engine.cpp, $ext_shared)
fi
