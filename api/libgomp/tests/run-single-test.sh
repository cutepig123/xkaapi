#! /bin/bash

if [ $# -eq 0 ]; 
then
    echo "Usage: ./run-single-test.sh testname [testargs]"
    exit
fi

KAAPI_INSTALL_PATH=/tmp/xkaapi

(uname -a | grep Darwin) > /dev/null 2> /dev/null
DARWIN_SYS=$?

if [ $DARWIN_SYS -eq 0 ]; 
then
    export DYLD_FORCE_FLAT_NAMESPACE=1
    export DYLD_INSERT_LIBRARIES="$KAAPI_INSTALL_PATH/lib/libkaapic.dylib:$KAAPI_INSTALL_PATH/lib/libgomp.dylib"
else
    export LD_PRELOAD="../.libs/libgomp.so"
fi

testname=$1
shift

# Run the test  
./$testname $@
