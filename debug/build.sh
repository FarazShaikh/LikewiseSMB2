#! /bin/bash

export CWD=`pwd`

export LW_FEATURE_LEVEL="auth"
export LSA_RPC_SERVERS="yes"

export CFLAGS="-w -Wno-error=unused-but-set-variable -Wno-error=implicit-function-declaration -Wno-error=sizeof-pointer-memaccess -Wno-error=unused-local-typedefs -Wno-error=pointer-sign -Wno-error=address"


echo "Compiles on VMWARE photo 1.0 distro"
echo "Compiles on VMWARE photo 1.0 distro"
echo "Compiles on VMWARE photo 1.0 distro"

#export DEBUG=--debug
../configure $DEBUG \
             --prefix=/opt/likewise \
             --libdir=/opt/likewise/lib64 \
             --datadir=/opt/likewise/share \
             --datarootdir=/opt/likewise/share \
             --build-isas=x86_64 \
             --lw-bundled-libs='libedit' \
             --enable-vmdir-provider=yes

make all package

