#!/bin/bash

# Usage: 
# $ ./runtime.sh <exe_loc> <exe_basename> <arg> <bench_name>
# EXE_LOCATION must contain exe + cl kernel

PROJECT_DIR=$(echo $PWD)

#echo "--- RUN TEST ---"
#./run_test.sh "$1" "$2" "$3" "$4"
#if [[ $? -ne 0 ]]
#then
#    exit
#fi

#echo "--- RUN EM ---"
#cd EM-framework
#./EM > $PROJECT_DIR/EM_out.txt

echo "--- RUN FINAL ---"
cd $PROJECT_DIR
./run_final.sh $PROJECT_DIR/EM_out.txt $PROJECT_DIR/ConfigList.txt "$1" "$2" "$3" "$4.txt"

echo "--- RUN FINISHED ---"
echo "[cfg...]  <millijoules> <microsec>:"
cat $4.txt
