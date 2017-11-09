#!/bin/bash

# Usage:
# ./run_final.sh <input> <ConfigList.txt> <exe_path> <exe_base_name> <args> <out>

PROJECT_DIR=$(echo $PWD)
ROOT_DIR=$PROJECT_DIR/rodinia_3.1/opencl/energywrapper/ocl

CFG_LINE=$(awk 'NR==1 { print $1 }' $1)
echo "$CFG_LINE"

declare -a CFG
CFG=($(awk -v l="$CFG_LINE" 'NR==l { print $0 }' $2))
echo "$CFG"

#CFG_GPU=($(echo "$CFG" | awk '{ print $1 }'))
#echo "${CFG_GPU[@]}"

cd $3
if [[ $CFG_LINE -gt 384 ]] ; then
	echo "Running on GPU with config: ${CFG[0]}"
	$ROOT_DIR/main_gpu "./$4_gpu" "$5" "${CFG[0]}" "$PROJECT_DIR/$6" &> /dev/null
else
	echo "Running on CPU with config: ${CFG[@]}"
	$ROOT_DIR/main_cpu "./$4_cpu" "$5" "${CFG[@]}" "$PROJECT_DIR/$6" &> /dev/null
fi
