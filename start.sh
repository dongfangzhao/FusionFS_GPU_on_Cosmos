#!/bin/sh
#LD_LIBRARY_PATH=/mnt/common/cshou/usr/include:/mnt/common/cshou/usr/lib:/mnt/common/cshou/FusionFS/fusionFS/src/ffsnet:/mnt/common/cshou/FusionFS/fusionFS/src/udt/src:/mnt/common/cshou/FusionFS/fusionFS/src/provenance
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

mkdir -p ./fusion_root
mkdir -p ./fusion_mount

./src/fusionfs ./fusion_root ./fusion_mount
