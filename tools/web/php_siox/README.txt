1. Install php-devel, compilers and tools

Inside the php_siox directory, execute the following commands:
 
2. phpize
3. ./configure --enable-siox
4. make install

You can see if the module is loaded with:
$php -d"extension=modules/siox.so" -m



