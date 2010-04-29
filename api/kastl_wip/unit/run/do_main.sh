#!/usr/bin/env sh


ALGOS="inner_product"
LIBS="stl kastl pastl"
DOS="bench check"

DIR=/tmp/out
#SIZE=1000000
#SIZE=1000000
SIZE=1000000
ITER=10
#CPUSET=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
#CPUSET=8,9,10,11,12,13,14,15
#CPUSET=0,1,2,3,4,5,6,7
#CPUSET=5,7,9,11
#CPUSET=7,9,11
#CPUSET=9,11
#CPUSET=9
CPUSET=0,1
#CPUSET=1


rm -rf $DIR
mkdir $DIR

#KAAPI_GLOBAL_OPTS=''
#KAAPI_GLOBAL_OPTS='KAAPI_WSSELECT=workload KAAPI_DISPLAY_PERF=1'
#KAAPI_GLOBAL_OPTS='KAAPI_WSSELECT=workload'
KAAPI_GLOBAL_OPTS=''
#KAAPI_GLOBAL_OPTS='KAAPI_DISPLAY_PERF=1'
#KAAPI_GLOBAL_OPTS='KAAPI_WSSELECT=workload'
#OUTPUT_FILTER=^.
OUTPUT_FILTER='-v KAAPI'
#OUTPUT_FILTER=^Total.idle
#OUTPUT_FILTER=Total.idle
#OUTPUT_FILTER='\[x\]'
#OUTPUT_FILTER=^work:

export LD_LIBRARY_PATH=$HOME/install/lib

for ALGO in $ALGOS; do
    for DO in $DOS; do
	for LIB in $LIBS; do
	    NAME=$ALGO-$LIB-$DO
	    if [ -e ../bin/$NAME ]; then
		OUTPUT_FILE=/tmp/oo/$NAME.o ;
		KAAPI_CPUSET=$CPUSET ../bin/$NAME $SIZE $ITER > $OUTPUT_FILE ;
		echo == ;
	    fi
	done
    done
done
