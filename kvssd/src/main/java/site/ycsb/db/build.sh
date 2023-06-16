#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "pass the unvme library path"
  exit -1
fi

UNVME_PATH=$1
UNVME_INCLUDE_PATH="${UNVME_PATH}/include"
UNVME_LIB_PATH="${UNVME_PATH}/bin/libuio.a"
UNVME_SPDK_INCLUDE_PATH="${UNVME_PATH}/driver/external/spdk/include"

INCLUDE_PATH="/usr/lib/jvm/java-11-openjdk-amd64/include"
ARC_INCLUDE_PATH="/usr/lib/jvm/java-11-openjdk-amd64/include/linux"

SRC_NAME="site_ycsb_db_KvssdUnvme.c"
COMPILED_NAME="site_ycsb_db_KvssdUnvme.o"
LIB_NAME="libKvssdUnvme.so"
LIB_COPY_PATH="/usr/lib/jni"

g++ -c -fPIC -I"${INCLUDE_PATH}" -I"${ARC_INCLUDE_PATH}" -I"${UNVME_INCLUDE_PATH}" -I"${UNVME_SPDK_INCLUDE_PATH}" ${SRC_NAME} -o ${COMPILED_NAME}
g++ -Wl,--whole-archive ${UNVME_LIB_PATH} -Wl,--no-whole-archive -shared -fPIC -o ${LIB_NAME} ${COMPILED_NAME} -lc -lnuma -luuid
rm ${COMPILED_NAME}
sudo mkdir -p ${LIB_COPY_PATH}
sudo mv ${LIB_NAME} ${LIB_COPY_PATH}/${LIB_NAME}

