LD_LIBRARY_PATH=/usr/local/include:/usr/local/lib:/home/dzhao/fusionfs/src/ffsnet:/home/dzhao/fusionfs/src/udt/src:/home/dzhao/fusionfs/src/provenance
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

#echo $LD_LIBRARY_PATH

#start the service for file transfer
./src/ffsnet/ffsnetd 2>&1 1>/dev/null &  

#start the service for ZHT server
./src/zht/bin/server_zht 50000 ./src/zht/neighbor ./src/zht/zht.cfg TCP 2>&1 1>/dev/null &

# start zht service for spade
./src/zht/bin/server_zht 55000 ./src/zht/spade_neighbor ./src/zht/zht.cfg TCP 2>&1 1>/dev/null &

#start the service for SPADE server
#/home/cshou/FusionFS/fusionFS/src/SPADE/bin/server start
