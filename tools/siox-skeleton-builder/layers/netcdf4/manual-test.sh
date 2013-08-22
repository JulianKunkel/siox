#!/bin/bash -e
./waf clean build  || exit 1
rm *.dat || echo -n ""

LD_PRELOAD=/usr/local/siox/lib/libsiox-posix-dlsym.so:build/libsiox-netcdf-dlsym.so ./build/pres_temp_4D_wr-dlsym
#gcc ./test/pres_temp_4D_wr.c -o a.out -Wall -ggdb   -L /usr/local/siox/lib/ -l siox-posix-dlsym -L build -l siox-netcdf-dlsym -lnetcdf
#./a.out
/home/julian/Dokumente/DKRZ/wr-git/wr-projects-sioxhh/Komponenten/build/tools/TraceReader/siox-trace-reader 

