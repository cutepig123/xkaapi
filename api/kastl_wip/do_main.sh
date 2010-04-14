#!/usr/bin/env sh


ALGO=ForEach
DIR=/tmp/out
#SIZE=1000000
#SIZE=1000000
SIZE=10000
ITER=10
#CPUSET=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
#CPUSET=8,9,10,11,12,13,14,15
#CPUSET=0,1,2,3,4,5,6,7
#CPUSET=5,7,9,11
CPUSET=7,9,11
#CPUSET=9,11
#CPUSET=9


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

export LD_LIBRARY_PATH=/home/lementec/install/lib

while true; do
#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=15 KAAPI_CPUCOUNT=1 ./main $ALGO $SIZE | grep $OUTPUT_FILTER
#    KAAPI_CPUSET=9,11,13,15 ./main $ALGO $SIZE $ITER
#    KAAPI_CPUSET=13,15 ./main $ALGO $SIZE $ITER
    KAAPI_CPUSET=$CPUSET ./main $ALGO $SIZE $ITER
#    sudo $KAAPI_GLOBAL_OPTS KAAPI_WSSELECT=workload KAAPI_CPUSET=15 KAAPI_CPUCOUNT=1 ./main $ALGO $SIZE | grep -v KAAPI

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;
#    sudo $KAAPI_GLOBAL_OPTS KAAPI_WSSELECT=workload KAAPI_CPUSET=13,15 KAAPI_CPUCOUNT=2 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;
#    sudo $KAAPI_GLOBAL_OPTS KAAPI_WSSELECT=workload KAAPI_CPUSET=11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=9,11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;
#    sudo $KAAPI_GLOBAL_OPTS KAAPI_WSSELECT=workload KAAPI_CPUSET=9,11,13,15 KAAPI ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=7,9,11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=5,7,9,11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=3,5,7,9,11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUSET=1,3,5,7,9,11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;
#    sudo $KAAPI_GLOBAL_OPTS KAAPI_WSSELECT=workload KAAPI_CPUSET=1,3,5,7,9,11,13,15 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;

#    sudo $KAAPI_GLOBAL_OPTS KAAPI_CPUCOUNT=16 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;
#    sudo $KAAPI_GLOBAL_OPTS KAAPI_WSSELECT=workload KAAPI_CPUCOUNT=16 ./main $ALGO $SIZE | grep $OUTPUT_FILTER ;
    echo == ;
done
