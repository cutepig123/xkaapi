#!/bin/bash

#SCRATCH=/scratch/jvlima
XKAAPIDIR=$HOME/install/xkaapi/default

CXX=g++

CFLAGS="-DKAAPI_DEBUG=0 -DKAAPI_NDEBUG=1 
-DCONFIG_USE_DOUBLE=1 -I$XKAAPIDIR/include
"
LDFLAGS="-L$XKAAPIDIR/lib -lkaapi -lkaapi++ -lgfortran"

#PLASMA_CFLAGS="-DCONFIG_USE_PLASMA=1 $PLASMA_CFLAGS"
#PLASMA_LDFLAGS="$PLASMA_LDFLAGS -lplasma"

#MAGMA_CFLAGS="-DCONFIG_USE_MAGMA=1 $MAGMA_CFLAGS"

CUBLAS_CFLAGS="-DCONFIG_USE_CUBLAS=1"
CUBLAS_LDFLAGS="-lcublas"
CUDA_CFLAGS="-DCONFIG_USE_CUDA=1 $CUDA_CFLAGS"

$CXX -g -Wall \
    $CFLAGS \
    $PLASMA_CFLAGS \
    $CUDA_CFLAGS \
    $CBLAS_CFLAGS \
    $CUBLAS_CFLAGS \
    $LAPACK_CLAGS \
    $LAPACKE_CFLAGS \
    $MAGMA_CFLAGS \
    -c matlu_right-looking_kaapi++.cpp 


$CXX -g \
    -o matlu_right-looking_kaapi++ \
    matlu_right-looking_kaapi++.o \
    $LDFLAGS \
    $MAGMA_LDFLAGS \
    $PLASMA_LDFLAGS \
    $CUDA_LDFLAGS \
    $CUBLAS_LDFLAGS \
    $LAPACKE_LDFLAGS \
    $LAPACK_LDFLAGS \
    $CBLAS_LDFLAGS 

