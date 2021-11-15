#!/bin/bash
set -e

RECONFIGURE=OFF

while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
    -r|--reconfigure)
        echo Reconfigure ON
        RECONFIGURE=ON
        shift # past argument with no value
        ;;

    -*|--*=) # unsupported flags
        echo "Error: Unsupported flag $1" >&2
        exit 1
        ;;
    *) # preserve positional arguments
        PARAMS="$PARAMS $1"
        shift
        ;;
  esac
done

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"


BUILD_PC=0
BUILD_ARM=1

if [ "${TARGET}" = "ARM" ]; then
    BUILD_TARGET_DIR=arm
    unset LD_LIBRARY_PATH
    source /home/krzysztof/Yocto/kdistrogl_sdk/environment-setup-cortexa72-kdistrogl-linux
else
    BUILD_PC=1
    BUILD_ARM=0
    BUILD_TARGET_DIR=pc
    CMAKE_OPTIONS="-DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_PREFIX_PATH=$HOME/Qt/6.2.0/gcc_64 "
fi


BUILD_SUFFIX="-debug"
BUILD_MAKE_TARGET="all"

BUILD_DIR=${SRC_DIR}/build/${BUILD_TARGET_DIR}${BUILD_SUFFIX}

if [ ${RECONFIGURE} = ON ] ; then
    rm -rf ${BUILD_DIR}/CMakeCache.txt
fi

THREADS=$((`nproc` + 2))

mkdir -p ${BUILD_DIR}

cmake -GNinja -DCMAKE_BUILD_TYPE=DEBUG ${CMAKE_OPTIONS} -S ${SRC_DIR} -B ${BUILD_DIR} -DBUILD_ARM=${BUILD_ARM} -DBUILD_PC=${BUILD_PC}
cmake --build ${BUILD_DIR} -j ${THREADS} -t ${BUILD_MAKE_TARGET}
