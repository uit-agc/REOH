#!/bin/bash

ROOT_DIR=$(echo $PWD)

#rm main_cpu
#make main_cpu

# BFS
#for i in {1..24}
#do
#   for j in {0..7}
#   do
#		for k in {0..1}
#		do
#			echo "Performing BFS on all datasets with ncpus $i, freqind $j, memctrl $k"
			# run 1
#			./main_cpu ../../bfs/bfs_cpu    ../../../data/bfs/graph4096.txt $i $j $k  $ROOT_DIR/output2/bfs/CPU/graph4096.log
#			./main_cpu ../../bfs/bfs_cpu    ../../../data/bfs/graph65536.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph65536.log
#			./main_cpu ../../bfs/bfs_cpu 	../../../data/bfs/graph1MW_6.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph1MW_6.log

			# run 2
#			./main_cpu ../../bfs/bfs_cpu 	../../../data/bfs/inputGen/graph512k.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph512k.log
#			./main_cpu ../../bfs/bfs_cpu 	../../../data/bfs/inputGen/graph1M.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph1M.log
#			./main_cpu ../../bfs/bfs_cpu 	../../../data/bfs/inputGen/graph2M.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph2M.log
#			./main_cpu ../../bfs/bfs_cpu 	../../../data/bfs/inputGen/graph4M.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph4M.log
#			./main_cpu ../../bfs/bfs_cpu 	../../../data/bfs/inputGen/graph8M.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph8M.log
#			./main_cpu ../../bfs/bfs_cpu 	../../../data/bfs/inputGen/graph16M.txt $i $j $k $ROOT_DIR/output2/bfs/CPU/graph16M.log
#		done
#   done
#done

# LUD
cd $ROOT_DIR/../../lud/ocl
for i in {1..24} # ncpus
do
    for j in {0..7} # freqind
    do
	for k in {0..1} # memctrl
	do
	    echo "Performing LUD on all datasets with ncpus $i, freqind $j, memctrl $k"
	    # run 1
	    #			$ROOT_DIR/main_cpu ./lud_cpu -i../../../data/_lud/64.dat $i $j $k   $ROOT_DIR/output2/_lud/CPU/64.log
	    #			$ROOT_DIR/main_cpu ./lud_cpu -i../../../data/_lud/256.dat $i $j $k  $ROOT_DIR/output2/_lud/CPU/256.log
	    #			$ROOT_DIR/main_cpu ./lud_cpu -i../../../data/_lud/512.dat $i $j $k  $ROOT_DIR/output2/_lud/CPU/512.log
	    #			$ROOT_DIR/main_cpu ./lud_cpu -i../../../data/_lud/2048.dat $i $j $k $ROOT_DIR/output2/_lud/CPU/2048.log

	    # run 2
	    $ROOT_DIR/main_cpu ./lud_cpu -s512 $i $j $k $ROOT_DIR/output2/qlud/CPU/512_generated.log
	    $ROOT_DIR/main_cpu ./lud_cpu -s1024 $i $j $k $ROOT_DIR/output2/qlud/CPU/1024_generated.log
	    $ROOT_DIR/main_cpu ./lud_cpu -s2048 $i $j $k $ROOT_DIR/output2/qlud/CPU/2048_generated.log
	    $ROOT_DIR/main_cpu ./lud_cpu -s4096 $i $j $k $ROOT_DIR/output2/qlud/CPU/4096_generated.log
	    $ROOT_DIR/main_cpu ./lud_cpu -s8192 $i $j $k $ROOT_DIR/output2/qlud/CPU/8192_generated.log
	done
    done
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
#cd $ROOT_DIR/../../kmeans
#for i in {1..24} # ncpus
#do
#   for j in {0..7} # freqind
#   do
#		for k in {0..1} # memctrl
#		do
#			echo "Performing KMEANS on all datasets with ncpus $i, freqind $j, memctrl $k"
			# run 1
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/204800.txt -l 2" $i $j $k $ROOT_DIR/output2/kmeans/CPU/204800.log
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/100 -l 2" $i $j $k        $ROOT_DIR/output2/kmeans/CPU/100.log
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/819200.txt -l 2" $i $j $k $ROOT_DIR/output2/kmeans/CPU/819200.log
			# run 2
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/inpuGen/1000_34.txt" $i $j $k   $ROOT_DIR/output2/kmeans/CPU/1000_34.log
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/inpuGen/10000_34.txt" $i $j $k   $ROOT_DIR/output2/kmeans/CPU/10000_34.log
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/inpuGen/100000_34.txt" $i $j $k  $ROOT_DIR/output2/kmeans/CPU/100000_34.log
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/inpuGen/1000000_34.txt" $i $j $k $ROOT_DIR/output2/kmeans/CPU/1000000_34.log
#			$ROOT_DIR/main_cpu ./kmeans_cpu "-i ../../data/kmeans/inpuGen/3000000_34.txt" $i $j $k $ROOT_DIR/output2/kmeans/CPU/3000000_34.log
#		done
#   done
#done

# CFD
#cd $ROOT_DIR/../../cfd
#for i in {1..24} # ncpus
#do
#   for j in {0..7} # freqind
#   do
#		for k in {0..1} # memctrl
#		do
#			echo "Performing CFD on all datasets $i $j $k"
			# run 1
#			$ROOT_DIR/main_cpu ./cfd_cpu "../../data/cfd/missile.domn.0.2M" $i $j $k $ROOT_DIR/output2/cfd/CPU/missile.domn.0.2M.log
#			$ROOT_DIR/main_cpu ./cfd_cpu "../../data/cfd/fvcorr.domn.097K" $i $j $k  $ROOT_DIR/output2/cfd/CPU/fvcorr.domn.097K.log
#			$ROOT_DIR/main_cpu ./cfd_cpu "../../data/cfd/fvcorr.domn.193K" $i $j $k  $ROOT_DIR/output2/cfd/CPU/fvcorr.domn.193K.log
#		done
#   done
#done

# PF
# ./OCL_particle_filter_double_gpu -x <dim_x> -y <dim_y> -z <n_frames> -np <num_particles>
#cd $ROOT_DIR/../../particlefilter
#for i in {1..24} # ncpus
#do
#    for j in {0..7} # freqind
#    do
#	for k in {0..1} # memctrl
#	do
#	    echo "Performing Particlefilter $i $j $k"
	    # run 1
	    #			$ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10 -np 400000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10_400000.log

	    # -- estimates from pf paper --
	    # 10k particles
	    #			$ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10000 -np 1000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10000_1000.log
	    #			$ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10000 -np 2000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10000_2000.log
	    #			$ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10000 -np 3000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10000_3000.log

	    # 10 frames
	    #			$ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10 -np 10000" $i $j $k  $ROOT_DIR/output2/particlefilter/CPU/128_10_10000.log
	    #			$ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10 -np 50000" $i $j $k  $ROOT_DIR/output2/particlefilter/CPU/128_10_50000.log
	    #			$ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10 -np 100000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10_100000.log

	    # run 2
	    # 10 frames
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10 -np 1000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10_1000_dp.log
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10 -np 10000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10_10000_dp.log
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 10 -np 100000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_10_100000_dp.log
	    # 10,000 particles
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 500 -np 10000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_500_10000_dp.log
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 1000 -np 10000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_1000_10000_dp.log
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 1500 -np 10000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_1500_10000_dp.log
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 2000 -np 10000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_2000_10000_dp.log
#	    $ROOT_DIR/main_cpu ./OCL_particlefilter_double_cpu "-x 128 -y 128 -z 2500 -np 10000" $i $j $k $ROOT_DIR/output2/particlefilter/CPU/128_2500_10000_dp.log
#	done
#    done
#done

# NW
#cd $ROOT_DIR/../../nw
#for i in {1..24} # ncpus
#do
#   for j in {0..7} # freqind
#   do
#		for k in {0..1} # memctrl
#		do
			# 65536 seems to be the limit (segfaults)
#			echo "Performing NW $i $j $k"
#			$ROOT_DIR/main_cpu ./nw_cpu "2048 10 ./nw.cl" $i $j $k $ROOT_DIR/output2/nw/CPU/2048_10.log
#			$ROOT_DIR/main_cpu ./nw_cpu "4096 10 ./nw.cl" $i $j $k $ROOT_DIR/output2/nw/CPU/4096_10.log
#			$ROOT_DIR/main_cpu ./nw_cpu "8192 10 ./nw.cl" $i $j $k $ROOT_DIR/output2/nw/CPU/8192_10.log
#			$ROOT_DIR/main_cpu ./nw_cpu "16384 10 ./nw.cl" $i $j $k $ROOT_DIR/output2/nw/CPU/16384_10.log
#			$ROOT_DIR/main_cpu ./nw_cpu "32768 10 ./nw.cl" $i $j $k $ROOT_DIR/output2/nw/CPU/32768_10.log
#		done
#   done
#done
