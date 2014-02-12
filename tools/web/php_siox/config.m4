PHP_ARG_WITH(siox, 
	[for siox support],
	[  --with-siox           Include siox support])

PHP_ARG_ENABLE(siox, 
	[whether to enable siox support],
	[  --enable-siox           Enable siox support])

if test "$PHP_SIOX" != "no"; then
	SEARCH_PATH="/usr/local /usr /opt"
	SEARCH_FOR="/include/C/siox.h"
	if test -r $PHP_SIOX/$SEARCH_FOR
	then
		SIOX_DIR=$PHP_SIOX
	else
		AC_MSG_CHECKING([for siox files in default path])
		for i in $SEARCH_PATH
		do
			if test -r $i/$SEARCH_FOR
			then
				SIOX_DIR=$i
				AC_MSG_RESULT(found in $i)
			fi
		done
	fi

	if test -z "$SIOX_DIR"
	then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([Please (re)install SIOX])
	fi

	PHP_ADD_INCLUDE($SIOX_DIR/include)

dnl	LIBNAME=siox
dnl	LIBSYMBOL=siox

dnl	PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
dnl	[
dnl		PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SIOX_DIR/lib, SIOX_SHARED_LIBADD)
dnl		AC_DEFINE(HAVE_SIOXLIB,1,[ ])
dnl	],[
dnl		AC_MSG_ERROR([wrong siox lib version or lib not found])
dnl	],[
dnl		-L$SIOX_DIR/lib -lm
dnl	])

	PHP_SUBST(SIOX_SHARED_LIBADD)

	PHP_REQUIRE_CXX()
	
	PHP_ADD_LIBRARY(stdc++, 1, SIOX_SHARED_LIBADD)
	PHP_NEW_EXTENSION(siox, php_siox.cpp siox_engine.cpp, $ext_shared, ,"-Wall -std=gnu++11")
fi
