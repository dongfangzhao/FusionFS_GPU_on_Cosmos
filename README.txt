###############################################
#DFZ: add the following ENVs to your ~/.bashrc#
###############################################

#for binary and runtime library 
export PATH=/usr/local/cuda-5.0/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda-5.0/lib64:/usr/local/include:/usr/local/lib:/home/dzhao/fusionfs/src/ffsnet:/home/dzhao/fusionfs/src/udt/src:/home/dzhao/fusionfs/src/provenance:$LD_LIBRARY_PATH

#for the gibraltar GPU library
export GIB_CACHE_DIR=/home/dzhao/fusionfs/src/gibraltar/cache
export GIB_SRC_DIR=/home/dzhao/fusionfs/src/gibraltar/src

#for CUDA
export CUDA_INC_PATH=/usr/local/cuda-5.0/include
export CUDA_LIB_PATH=/usr/local/cuda-5.0/lib64

#DFZ: Cosmos (our GPU cluster) doesn't have NFS... the following should work to synchronize code edit/compilation/execution.
#     Suppose p1 is the manager, and p3 - p10 are the work nodes. Edit source file on p1 and run experiments on p3 - p10:     
vi src/fusionfs.c
pssh -h hosts "cd ~/fusionfs; ./stop.sh"
pscp -h hosts src/fusionfs.c ~/fusionfs/src/
pssh -h hosts "cd ~/fusionfs/src; make"
pssh -h hosts "cd ~/fusionfs; ./start.sh"
