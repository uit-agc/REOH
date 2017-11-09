#!/bin/bash

# Usage
# ./bench_cpu.sh <ocl kernel & exe path> <exe_base_name> <exe argument> <bench_name>
# E.g
# ./bench_cpu.sh ../../particlefilter/ OCL_particlefilter_double "-x 128 -y 128 -z 1000 -np 10000" 128_1000_10000

# arg1 is exe file (exclude _cpu)
# arg2 string to contain argument to exe file
# arg3 is name of bench, output will be stored in test_output

# ------------------------------
# MODIFY THESE

# CPU configurations

# CFG1
typeset -a CPUS=("1 4 7 10 14 17 21 24")
typeset -a FREQINDS=("0 3 7")
typeset -a MEMCTRLS=("0 1")

# CFG2
#typeset -a CPUS=("1 3 5 7 9 11 13 15 17 19 21 23")
#typeset -a FREQINDS=("0 5")
#typeset -a MEMCTRLS=("0")

# CFG 3
#typeset -a CPUS=("2")
#typeset -a FREQINDS=("2")
#typeset -a MEMCTRLS=("0 1")

# GPU configurations
typeset -a GPU_SIZES=("1 256")


# ------------------------------

PROJECT_DIR=$(echo $PWD)
ROOT_DIR=$PROJECT_DIR/rodinia_3.1/opencl/energywrapper/ocl

mkdir $PROJECT_DIR/Data/test

OUT=$PROJECT_DIR/Data/test/$4

if [ -d "$OUT" ] ; then
	echo "bench already exist"
	exit 1
fi

mkdir $OUT

OUTPUT_FOLDER_CPU=$PROJECT_DIR/Data/test/$4/CPU
mkdir $OUTPUT_FOLDER_CPU
mkdir $OUTPUT_FOLDER_CPU/leo

OUTPUT_FOLDER_GPU=$PROJECT_DIR/Data/test/$4/GPU
mkdir $OUTPUT_FOLDER_GPU
mkdir $OUTPUT_FOLDER_GPU/leo

# TODO: run test_ene_cpu and test_ene_gpu instead
declare -i GPU_STATIC_ENE
declare -i CPU_STATIC_ENE
GPU_STATIC_ENE=625936 # mJ measured separately
CPU_STATIC_ENE=1067768 # mJ measured separately
GPU_STATIC_POW=$[($GPU_STATIC_ENE/1000)/20] # convert to Jouls and divide by 20sec
CPU_STATIC_POW=$[($CPU_STATIC_ENE/1000)/20] # convert to Jouls and divide by 20sec

# Run CPU benchmark
cd $1
for i in {1..24}
do
	for j in {0..7}
	do
		for k in {0..1}
		do
			skip_print=false
			if [[ " ${CPUS[@]} " =~  " $i " ]] ; then
				if [[ " ${FREQINDS[@]} " =~  " $j " ]] ; then							  
					if [[ " ${MEMCTRLS[@]} " =~  " $k " ]] ; then
						echo "Running on CPU with config $i $j $k"
						$ROOT_DIR/main_cpu "./$2_cpu" "$3" $i $j $k $OUTPUT_FOLDER_CPU/$4.log &> /dev/null
						skip_print=true
					fi
				fi
			fi
			if [[ "$skip_print" =~ false ]] ; then
				echo -e "0\t0\t0\t0\t0" >> $OUTPUT_FOLDER_CPU/$4.log
			fi
		done
	done
done


# ** CPU leo formating **
# NOTE! This will not give static energy measures unless we test it explicitly here.
# Generate Testnames.txt
echo "'$4'" >> $OUTPUT_FOLDER_CPU/leo/Testnames.txt
# Generate X.txt
awk '{ print $1 "\t" $2 "\t" $3}' $OUTPUT_FOLDER_CPU/$4.log >> $OUTPUT_FOLDER_CPU/leo/X.txt
# Generate TestX.txt
awk '{ if ($1 == 0) { print 0 } else { print 1 } }' $OUTPUT_FOLDER_CPU/leo/X.txt >> $OUTPUT_FOLDER_CPU/leo/TestX.txt
# Generate TestPerformance.txt
awk '{ if ($5 != 0) { print $5 / 1000000 } else { print 0 } }' $OUTPUT_FOLDER_CPU/$4.log >> $OUTPUT_FOLDER_CPU/leo/TestPerformance.txt
# Generate TestPower.txt
awk -v gpupow="$GPU_STATIC_POW" '{ if ($5 != 0) { print ($4 / 1000) / ($5 / 1000000) + gpupow } else { print 0 } }' $OUTPUT_FOLDER_CPU/$4.log >> $OUTPUT_FOLDER_CPU/leo/TestPower.txt
# Generate TestEnergy 
awk -v gpupow="$GPU_STATIC_POW" '{ if ($4 != 0) { print (($4 / 1000) / ($5 / 1000000) + gpupow) * ($5 / 1000000) } else { print 0 } }' $OUTPUT_FOLDER_CPU/$4.log >> $OUTPUT_FOLDER_CPU/leo/TestEnergy.txt


# Run GPU benchmark
for i in {1,4,16,64,256}
do
	if [[ " ${GPU_SIZES[@]} " =~  " $i " ]] ; then
		echo "Running on GPU with config $i"
		$ROOT_DIR/main_gpu "./$2_gpu" "$3" $i $OUTPUT_FOLDER_GPU/$4.log &> /dev/null
	else
		echo -e "0\t0\t0" >> $OUTPUT_FOLDER_GPU/$4.log
	fi
done

# ** GPU leo formating **
# NOTE! This will not give static energy measures unless we test it explicitly here.
# Generate Testnames.txt
echo "'$4'" >> $OUTPUT_FOLDER_GPU/leo/Testnames.txt
# Generate X.txt
awk '{ print $1 }' $OUTPUT_FOLDER_GPU/$4.log >> $OUTPUT_FOLDER_GPU/leo/X.txt
# Generate TestX.txt
awk '{ if ($1 == 0) { print 0 } else { print 1 } }' $OUTPUT_FOLDER_GPU/leo/X.txt >> $OUTPUT_FOLDER_GPU/leo/TestX.txt
# Generate TestPerformance.txt
awk '{ if ($3 != 0) { print $3 / 1000000 } else { print 0 }  }' $OUTPUT_FOLDER_GPU/$4.log >> $OUTPUT_FOLDER_GPU/leo/TestPerformance.txt
# Generate TestPower.txt
awk -v cpupow="$CPU_STATIC_POW" '{ if ($3 != 0) { print ($2 / 1000) / ($3 / 1000000) + cpupow } else { print 0 } }' $OUTPUT_FOLDER_GPU/$4.log >> $OUTPUT_FOLDER_GPU/leo/TestPower.txt
# Generate TestEnergy 
awk -v cpupow="$CPU_STATIC_POW" '{ if ($2 != 0) { print (($2 / 1000) / ($3 / 1000000) + cpupow) * ( $3 / 1000000) } else { print 0 } }' $OUTPUT_FOLDER_GPU/$4.log >> $OUTPUT_FOLDER_GPU/leo/TestEnergy.txt

# copy CPU results to global output folder
cp $OUTPUT_FOLDER_CPU/leo/* $OUT

# append GPU results to the CPU results
awk '{ print $1 "\t8\t0" }' $OUTPUT_FOLDER_GPU/leo/X.txt >> $OUT/X.txt
cat $OUTPUT_FOLDER_GPU/leo/TestPerformance.txt >> $OUT/TestPerformance.txt
cat $OUTPUT_FOLDER_GPU/leo/TestPower.txt >> $OUT/TestPower.txt
cat $OUTPUT_FOLDER_GPU/leo/TestEnergy.txt >> $OUT/TestEnergy.txt
cat $OUTPUT_FOLDER_GPU/leo/TestX.txt >> $OUT/TestX.txt

# place latest data in /Data/test for EM framework
rm -rf $PROJECT_DIR/Data/test/TestEnergy.txt 
rm -rf $PROJECT_DIR/Data/test/Testnames.txt 
rm -rf $PROJECT_DIR/Data/test/TestPerformance.txt 
rm -rf $PROJECT_DIR/Data/test/TestPower.txt 
rm -rf $PROJECT_DIR/Data/test/TestX.txt 
rm -rf $PROJECT_DIR/Data/test/X.txt

cp $OUT/TestEnergy.txt $OUT/TestPerformance.txt $OUT/X.txt $OUT/TestX.txt $OUT/TestPower.txt $OUT/Testnames.txt $PROJECT_DIR/Data/test/

cp $OUT/* $PROJECT_DIR/Data/test/

exit 0
