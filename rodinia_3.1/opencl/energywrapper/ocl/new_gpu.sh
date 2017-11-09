#!/bin/bash

ROOT_DIR=$(echo $PWD)

#rm main_gpu
#make main_gpu

# BFS
for i in {1,2,4,8,16,32,64,128,256}
do
	export BFS_MAX_THREADS_PER_BLOCK=$i
	echo "Performing BFS with $BFS_MAX_THREADS_PER_BLOCK threads per block"
	# run 1
#	./main_gpu ../../bfs/bfs_gpu    ../../../data/bfs/graph4096.txt $i  $ROOT_DIR/output3/bfs/GPU/graph4096.log
#	./main_gpu ../../bfs/bfs_gpu    ../../../data/bfs/graph65536.txt $i $ROOT_DIR/output3/bfs/GPU/graph65536.log
#	./main_gpu ../../bfs/bfs_gpu    ../../../data/bfs/graph1MW_6.txt $i $ROOT_DIR/output3/bfs/GPU/graph1MW_6.log

	# run 2
	./main_gpu ../../bfs/bfs_gpu 	../../../data/bfs/inputGen/graph512k.txt $i $ROOT_DIR/output3/bfs/GPU/graph512k.log
	./main_gpu ../../bfs/bfs_gpu 	../../../data/bfs/inputGen/graph1M.txt $i   $ROOT_DIR/output3/bfs/GPU/graph1M.log
	./main_gpu ../../bfs/bfs_gpu 	../../../data/bfs/inputGen/graph2M.txt $i   $ROOT_DIR/output3/bfs/GPU/graph2M.log
	./main_gpu ../../bfs/bfs_gpu 	../../../data/bfs/inputGen/graph4M.txt $i   $ROOT_DIR/output3/bfs/GPU/graph4M.log
	./main_gpu ../../bfs/bfs_gpu 	../../../data/bfs/inputGen/graph8M.txt $i   $ROOT_DIR/output3/bfs/GPU/graph8M.log
	./main_gpu ../../bfs/bfs_gpu 	../../../data/bfs/inputGen/graph16M.txt $i  $ROOT_DIR/output3/bfs/GPU/graph16M.log
done 

# LUD
# Processes the matrix block by block, max local work group size is $i * $i
cd $ROOT_DIR/../../lud/ocl
for i in {1,2,4,8,16}
do
	export LUD_BLOCK_SIZE=$i
	echo "Performing LUD with BLOCK_SIZE = $i"
	# run 1
#	$ROOT_DIR/main_gpu ./lud_gpu -i../../../data/lud/64.dat $i   $ROOT_DIR/output3/lud/GPU/64.log
#	$ROOT_DIR/main_gpu ./lud_gpu -i../../../data/lud/256.dat $i  $ROOT_DIR/output3/lud/GPU/256.log
#	$ROOT_DIR/main_gpu ./lud_gpu -i../../../data/lud/512.dat $i  $ROOT_DIR/output3/lud/GPU/512.log
#	$ROOT_DIR/main_gpu ./lud_gpu -i../../../data/lud/2048.dat $i $ROOT_DIR/output3/lud/GPU/2048.log
	# run 2
	$ROOT_DIR/main_gpu ./lud_gpu -s512 $i $ROOT_DIR/output3/qlud/GPU/512_generated.log
	$ROOT_DIR/main_gpu ./lud_gpu -s1024 $i $ROOT_DIR/output3/qlud/GPU/1024_generated.log
	$ROOT_DIR/main_gpu ./lud_gpu -s2048 $i $ROOT_DIR/output3/qlud/GPU/2048_generated.log
	$ROOT_DIR/main_gpu ./lud_gpu -s4096 $i $ROOT_DIR/output3/qlud/GPU/4096_generated.log
	$ROOT_DIR/main_gpu ./lud_gpu -s8192 $i $ROOT_DIR/output3/qlud/GPU/8192_generated.log
done

# KMEANS
# Usage: %s [switches] -i filename
# -i filename      :file containing data to be clustered
# -m max_nclusters :maximum number of clusters allowed    [default=5]
# -n min_nclusters :minimum number of clusters allowed    [default=5]
# -l nloops        :iteration for each number of clusters [default=1]
# -b               :input file is in binary format
# -r               :calculate RMSE                        [default=off]
# -o               :output cluster center coordinates     [default=off]
cd $ROOT_DIR/../../kmeans
for i in {1,2,4,8,16,32,64,128,256}
do
	export KMEANS_BLOCK_SIZE=$i
	export KMEANS_BLOCK_SIZE2=$i
	echo "Performing KMEANS with BLOCK_SIZE's = $i"
	# run 1
#	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/100 -l 2" $i        $ROOT_DIR/output3/kmeans/GPU/100.log
#	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/204800.txt -l 2" $i $ROOT_DIR/output3/kmeans/GPU/204800.log
#	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/819200.txt -l 2" $i $ROOT_DIR/output3/kmeans/GPU/819200.log

	# run 2
	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/inpuGen/1000_34.txt" $i   $ROOT_DIR/output3/kmeans/GPU/1000_34.log
	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/inpuGen/10000_34.txt" $i   $ROOT_DIR/output3/kmeans/GPU/10000_34.log
	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/inpuGen/100000_34.txt" $i  $ROOT_DIR/output3/kmeans/GPU/100000_34.log
	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/inpuGen/1000000_34.txt" $i $ROOT_DIR/output3/kmeans/GPU/1000000_34.log
	$ROOT_DIR/main_gpu ./kmeans_gpu "-i ../../data/kmeans/inpuGen/3000000_34.txt" $i $ROOT_DIR/output3/kmeans/GPU/3000000_34.log	
done

# CFD
cd $ROOT_DIR/../../cfd
for i in {1,2,4,8,16,32,64,128,256}
do
	export CFD_BLOCK_SIZE_0=$i
	export CFD_BLOCK_SIZE_1=$i
	export CFD_BLOCK_SIZE_2=$i
	export CFD_BLOCK_SIZE_3=$i
	export CFD_BLOCK_SIZE_4=$i
	echo "Performing CFD with BLOCK_SIZE's = $i"
	# run 1
	$ROOT_DIR/main_gpu ./cfd_gpu "../../data/cfd/missile.domn.0.2M" $i  $ROOT_DIR/output3/cfd/GPU/missile.domn.0.2M.log
	$ROOT_DIR/main_gpu ./cfd_gpu "../../data/cfd/fvcorr.domn.097K" $i  $ROOT_DIR/output3/cfd/GPU/fvcorr.domn.097K.log
	$ROOT_DIR/main_gpu ./cfd_gpu "../../data/cfd/fvcorr.domn.193K" $i $ROOT_DIR/output3/cfd/GPU/fvcorr.domn.193K.log

	# run 2
done


# PF
# ./OCL_particle_filter_double_gpu -x <dim_x> -y <dim_y> -z <n_frames> -np <num_particles>
cd $ROOT_DIR/../../particlefilter
for i in {1,2,4,8,16,32,64,128,256}
do
	export PF_THREADS_PER_BLOCK=$i
	echo "Performing Particlefilter with $i threads per block"
	#run 1
#	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10 -np 400000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10_400000.log

	# -- estimates from pf paper --
	# 10k frames
#	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10000 -np 1000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10000_1000.log
#	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10000 -np 2000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10000_2000.log
#	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10000 -np 3000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10000_3000.log

	# 10 frames
#	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10 -np 10000" $i  $ROOT_DIR/output3/particlefilter/GPU/128_10_10000.log
#	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10 -np 50000" $i  $ROOT_DIR/output3/particlefilter/GPU/128_10_50000.log
#	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10 -np 100000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10_100000.log

	# run 2
	# 10 frames
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10 -np 1000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10_1000_dp.log
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10 -np 10000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10_10000_dp.log
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 10 -np 100000" $i $ROOT_DIR/output3/particlefilter/GPU/128_10_100000_dp.log
	# 10,000 particles
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 500 -np 10000" $i $ROOT_DIR/output3/particlefilter/GPU/128_500_10000_dp.log
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 1000 -np 10000" $i $ROOT_DIR/output3/particlefilter/GPU/128_1000_10000_dp.log
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 1500 -np 10000" $i $ROOT_DIR/output3/particlefilter/GPU/128_1500_10000_dp.log
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 2000 -np 10000" $i $ROOT_DIR/output3/particlefilter/GPU/128_2000_10000_dp.log
	$ROOT_DIR/main_gpu ./OCL_particlefilter_double_gpu "-x 128 -y 128 -z 2500 -np 10000" $i $ROOT_DIR/output3/particlefilter/GPU/128_2500_10000_dp.log
done

# NW
#cd $ROOT_DIR/../../nw
#for i in {1,4,8,16,32,64}
#do
#	export NW_BLOCK_SIZE=$i
#	echo "Performing NW with NW_BLOCK_SIZE=$i"
#	$ROOT_DIR/main_gpu ./nw_gpu "2048 10 ./nw.cl"  $i $ROOT_DIR/output3/nw/GPU/2048_10.log
#	$ROOT_DIR/main_gpu ./nw_gpu "4096 10 ./nw.cl"  $i $ROOT_DIR/output3/nw/GPU/4096_10.log
#	$ROOT_DIR/main_gpu ./nw_gpu "8192 10 ./nw.cl"  $i $ROOT_DIR/output3/nw/GPU/8192_10.log
#	$ROOT_DIR/main_gpu ./nw_gpu "16384 10 ./nw.cl" $i $ROOT_DIR/output3/nw/GPU/16384_10.log
#	$ROOT_DIR/main_gpu ./nw_gpu "32768 10 ./nw.cl" $i $ROOT_DIR/output3/nw/GPU/32768_10.log
#done
