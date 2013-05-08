#!/bin/sh
#LD_LIBRARY_PATH=/mnt/common/cshou/usr/include:/mnt/common/cshou/usr/lib:/mnt/common/cshou/FusionFS/fusionFS/src/ffsnet:/mnt/common/cshou/FusionFS/fusionFS/src/udt/src:/mnt/common/cshou/FusionFS/fusionFS/src/provenance
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

mkdir -p /home/dzhao/fusionfs/fusion_root
mkdir -p /home/dzhao/fusionfs/fusion_mount

/home/dzhao/fusionfs/src/fusionfs /home/dzhao/fusionfs/fusion_root /home/dzhao/fusionfs/fusion_mount
