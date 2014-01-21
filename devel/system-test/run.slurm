#!/bin/bash
#SBATCH --time=06:00:00 
#SBATCH -p paper
#SBATCH -N 10
#SBATCH --error=job.err.%j --output=job.out.%j

# SETUP FOR the WR GROUP's CLUSTER

export SIOX=/home/kunkel/siox/install
export PATH=$SIOX/bin:$PATH
export LD_LIBRARY_PATH=$SIOX/lib:/opt/siox/boost/1.54/lib/
export SIOX_TEST=/home/kunkel/siox/devel/system-test/
export RUNDIR=$SIOX_TEST/$SLURM_JOBID

mkdir $RUNDIR

cd $RUNDIR
srun --ntasks-per-node=1 hostname > hosts.dat

echo 
echo 
echo "SETUP"
for I in $(cat hosts.dat) ; do
   cp $SIOX_TEST/daemon.conf $I.conf
   # perform needed modifications
   sed -i "s#.dat#$I.dat#" $I.conf
   srun -N 1 -w $I siox-daemon --configEntry=$RUNDIR/$I.conf &
done
# srun  --ntasks-per-node=1 siox-daemon --configEntry=$SIOX_TEST/daemon.conf

cp $SIOX_TEST/siox.conf .
cp $SIOX_TEST/hints* .
cp $SIOX_TEST/ior.conf .

FLAGS="$SIOX_TEST/IOR.normal -v -o /mnt/lustre/testfile -s 10 -i 1 -f ior.conf"

echo 
echo 
echo "DEFAULT IOR"

mpiexec -np 60 -bynode $FLAGS

echo 
echo 
echo "WITH SIOX"
mpiexec -np 60 -bynode -x LD_PRELOAD="$SIOX/lib/libsiox-mpi-dlsym.so $SIOX/lib/libsiox-posix-dlsym.so" $FLAGS

echo 
echo 
echo "CLEANUP"
for I in $(cat hosts.dat) ; do
  ssh $I killall siox-daemon
done
