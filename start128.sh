#!/bin/sh
LD_LIBRARY_PATH=/mnt/common/cshou/usr/include:/mnt/common/cshou/usr/lib:/mnt/common/cshou/FusionFS/fusionFS/src/ffsnet:/mnt/common/cshou/FusionFS/fusionFS/src/udt/src:/mnt/common/cshou/FusionFS/fusionFS/src/provenance
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

./src/fusionfs -o max_write=131072 -o max_read=131072 -o direct_io -o big_writes -o large_read ./fusion_root ./fusion_mount
