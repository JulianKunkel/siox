#/bin/bash
for I in 1 2 3 ; do
for STRIDE in 10240 20480 1024000 ; do
for THINK in 1000 5000 10000 15000; do

gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead.fadvise  -DFADVISE2  -DTIME_THINK=$THINK -DSTRIDE=$STRIDE
gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead.withoutfadvise  -DTIME_THINK=$THINK -DSTRIDE=$STRIDE
siox-inst posix wrap gcc -Wall -g ./fadvise-readAhead.c -std=gnu99 -lrt -o fadvise-readAhead.siox -DTIME_THINK=$THINK -DSTRIDE=$STRIDE

for type in fadvise withoutfadvise siox ; do

echo "Running experiment $type with time: $THINK and stride: $STRIDE"
echo 3 > /proc/sys/vm/drop_caches || exit 1

./fadvise-readAhead.$type /tmp/test >> results.$type-$THINK-$STRIDE 2>&1
done
done
done
done
