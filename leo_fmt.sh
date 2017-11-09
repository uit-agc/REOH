#!/bin/bash

PROJECT_DIR=$(echo $PWD)
ROOT_DIR=$PROJECT_DIR/rodinia_3.1/opencl/energywrapper/ocl

LEO_DATA=$PROJECT_DIR/Data/train/leo3
OUTPUT_FOLDER_CPU=$LEO_DATA/CPU
OUTPUT_FOLDER_GPU=$LEO_DATA/GPU
APPENDED_OUT=$LEO_DATA

DATA_DIR=$ROOT_DIR/output3

rm -rf $LEO_DATA
mkdir $LEO_DATA
mkdir $OUTPUT_FOLDER_CPU
mkdir $OUTPUT_FOLDER_GPU

# --- CPU ---
# Generate X.txt from sample file
cd $DATA_DIR/bfs/CPU/
cat graph1M.log | awk '{print $1 "\t" $2 "\t" $3}' >> $OUTPUT_FOLDER_CPU/X.txt

files=""
for filepath in $DATA_DIR/*/CPU/*
do
	fname=${filepath##*/}
	dataset="${fname%.log}"
	gp=$(dirname $(dirname "$filepath"))
	algo=${gp##*/}
	name="$algo-$dataset"

	# Generate AllNames.txt
	echo "'$name'" >> $OUTPUT_FOLDER_CPU/Allnames.txt

	files+="$filepath "
done

# TODO: <run test_ene_cpu> and <test_ene_gpu> instead
declare -i GPU_STATIC_ENE
declare -i CPU_STATIC_ENE
GPU_STATIC_ENE=625936 # mJ measured separately
CPU_STATIC_ENE=1067768 # mJ measured separately
GPU_STATIC_POW=$[(GPU_STATIC_ENE/1000)/20] # convert to Jouls and divide by 20sec
CPU_STATIC_POW=$[(CPU_STATIC_ENE/1000)/20] # convert to Jouls and divide by 20sec

# Generate static.txt 
echo $GPU_STATIC_POW >> $OUTPUT_FOLDER_GPU/static.txt
echo $CPU_STATIC_POW >> $OUTPUT_FOLDER_GPU/static.txt

# $5 = runtime = microseconds
# $4 = energy = millijoules

# Generate AllPerformance.txt (i.e. runtime)
awk -F $'\t' '{ a[FNR] = (a[FNR] ? a[FNR] FS : "") $5 / 1000000 } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_CPU/AllPerformance.txt

# Generate AllPower.txt
awk -F $'\t' '{ a[FNR] = (a[FNR] ? a[FNR] FS : "") ($4 / 1000) / ($5 / 1000000) } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_CPU/AllPower.txt

# Generate AllPower_total.txt
awk -F $'\t' -v gpupow="$GPU_STATIC_POW" '{ a[FNR] = (a[FNR] ? a[FNR] FS : "") ($4 / 1000) / ($5 / 1000000) + gpupow } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_CPU/AllPower_total.txt

# Generate AllEnergy
awk -F $'\t' '{ a[FNR] = (a[FNR] ? a[FNR] FS : "") ($4 / 1000) } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_CPU/AllEnergy.txt

# Generate AllEnergy_total
awk -F $'\t' -v gpupow="$GPU_STATIC_POW" '{ a[FNR] = (a[FNR] ? a[FNR] FS : "")  (($4 / 1000) / ($5 / 1000000) + gpupow) * ($5 / 1000000) } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_CPU/AllEnergy_total.txt

# --- GPU ---
# TODO: Lud need some manual fixing
cd $DATA_DIR/bfs/GPU/
cat graph1M.log | awk '{print $1}' >> $OUTPUT_FOLDER_GPU/X.txt

files=""
for filepath in $DATA_DIR/*/GPU/*
do
	fname=${filepath##*/}
	dataset="${fname%.log}"
	gp=$(dirname $(dirname "$filepath"))
	algo=${gp##*/}
	name="$algo-$dataset"

	# Generate AllNames.txt
	echo "'$name'" >> $OUTPUT_FOLDER_GPU/Allnames.txt

	files+="$filepath "
done

# $3 = runtime = microseconds
# $2 = energy = millijoules

# Generate AllPerformance.txt (i.e. runtime)
awk -F $'\t' '/^\s*$/ {next;} { a[FNR] = (a[FNR] ? a[FNR] FS : "") $3 / 1000000 } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_GPU/AllPerformance.txt

# Generate AllPower.txt
awk -F $'\t' '/^\s*$/ {next;} { a[FNR] = (a[FNR] ? a[FNR] FS : "") ($2 / 1000) / ($3 / 1000000)  } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_GPU/AllPower.txt

#AllPower_total.txt is not correct for GPU.
# Generate AllPower_total.txt
awk -F $'\t' -v cpupow="$CPU_STATIC_POW" '/^\s*$/ {next;} { a[FNR] = (a[FNR] ? a[FNR] FS : "") ($2 / 1000) / ($3 / 1000000) + cpupow } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_GPU/AllPower_total.txt


# Generate AllEnergy.txt
awk -F $'\t' '/^\s*$/ {next;} { a[FNR] = (a[FNR] ? a[FNR] FS : "") ($2 / 1000) } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_GPU/AllEnergy.txt

# Generate AllEnergy_total.txt
awk -F $'\t' -v cpupow="$CPU_STATIC_POW" '/^\s*$/ {next;} { a[FNR] = (a[FNR] ? a[FNR] FS : "")  (($2 / 1000) / ($3 / 1000000) + cpupow) * ($3 / 1000000) } END { for(i=1;i<=FNR;i++) print a[i] }' $files >> $OUTPUT_FOLDER_GPU/AllEnergy_total.txt


# copy CPU results to appended output
cp $OUTPUT_FOLDER_CPU/* $APPENDED_OUT

awk '{ print $1 "\t8\t0" }' $OUTPUT_FOLDER_GPU/X.txt >> $APPENDED_OUT/X.txt
cat $OUTPUT_FOLDER_GPU/AllEnergy.txt >> $APPENDED_OUT/AllEnergy.txt
cat $OUTPUT_FOLDER_GPU/AllEnergy_total.txt >> $APPENDED_OUT/AllEnergy_total.txt
cat $OUTPUT_FOLDER_GPU/AllPerformance.txt >> $APPENDED_OUT/AllPerformance.txt
cat $OUTPUT_FOLDER_GPU/AllPower.txt >> $APPENDED_OUT/AllPower.txt
cat $OUTPUT_FOLDER_GPU/AllPower_total.txt >> $APPENDED_OUT/AllPower_total.txt
cat $OUTPUT_FOLDER_GPU/static.txt >> $APPENDED_OUT/static.txt

