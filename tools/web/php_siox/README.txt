1. Install php-devel, compilers and tools
2. phpize
3. ./configure --enable-siox
4. make install

You can see if the module is loaded with:
$php -d"extension=siox.so" -m



