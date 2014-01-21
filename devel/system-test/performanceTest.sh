#!/bin/bash -x

#SBATCH --time=03:00:00 
#SBATCH -p paper
#SBATCH -N 1
#SBATCH --error=performanceTest.err --output=performanceTest.out

export SIOX=/home/kunkel/siox/install
export PATH=$SIOX/bin:$PATH
export LD_LIBRARY_PATH=$SIOX/lib:/opt/siox/boost/1.54/lib/
export SIOX_TEST=/home/kunkel/siox/devel/system-test/

export RUNDIR=$SIOX_TEST/performanceTest

mkdir $RUNDIR || true
cd $RUNDIR

echo "Testing vanilla performance"
for I in `seq 1 12` ; do 
	FILE=$I-withoutSIOX
	if [[ ! -e $FILE ]] ; then
	../performance-test-multi-threaded 10 $I > $FILE
	fi
done

for C in siox siox-posix-fw siox-plain; do
	echo "Testing with SIOX config $C"
	CONFIG=../$C.conf
	cp $CONFIG siox.conf
	for I in `seq 1 12` ; do 
		rm *.dat || true
		FILE=$I-$C
		if [[ ! -e $FILE ]] ; then
		siox-inst posix ../performance-test-multi-threaded 10 $I > $FILE
		fi
	done
done


# prepare configuration to send.dat || truea to the daemon 
cp ../siox.conf siox.conf

killall siox-daemon

for C in daemon ; do
	echo "Testing SIOX with daemon config $C"
	CONFIG=../$C.conf
	cp $CONFIG daemon.conf

	for I in `seq 1 12` ; do 
		FILE=$I-daemon-$C
                if [[ ! -e $FILE ]] ; then

		rm *.dat || true
		rm /tmp/*socket
		siox-daemon --configEntry=$RUNDIR/daemon.conf &
		sleep 1

		siox-inst posix ../performance-test-multi-threaded 10 $I > $FILE
		sleep 1
		killall siox-daemon
		ls -la >> $FILE
		fi
	done
done



for I in $(ls |grep -v "\." |cut -d "-" -f 2- |sort|uniq) ; do
FILE=out-$I.txt
rm $FILE 2>/dev/null|| true
for C in $(seq 1 12) ;  do
	grep -C 1 "^Total time" $C-$I | tail -n 1 >> $FILE 2>/dev/null
done 
if [[ -s $FILE ]] ; then 
	rm $FILE 
fi
done

