Training phase:
$ run_all.sh # runs training scripts {new_cpu.sh, new_gpu.sh} -> makes energywrapper/ocl/output
Training scripts are coded to run 389 permutations with different configurations of freqind, memctrl, ncpus, and WG sizes for GPUs - using different training sets.
The scripts take no arguments, users may add or remove configs/inputs as they see fit.

Formaters:
$ ./leo_fmt.sh # will take data in energywrapper/ocl/output and format them for leo in energywrapper/ocl/leo_data
$ ./excel_fmt.sh # will format everything so that it can be easily imported to excel

Example testing phase:
# ./bench_cpu.sh <ocl kernel & exe path> <exe_base_name> <exe argument> <bench_name>. E.g:
$ ./run_output_bench.sh ../../particlefilter OCL_particlefilter_double "-x 128 -y 128 -z 1000 -np 10000" 128_1000_10000_t1
Assumes the names are OCL_particlefilter_double_cpu / OCL_particlefilter_double_gpu
Now appended (global) data will be in energywrapper/ocl/output_bench/128_1000/10000_t1/

energywrapper/ocl/output_bench/128_1000/10000_t1/ now contains data that fits Leo (appended tables are outside CPU and GPU folders.

To run with final application stored ./out.txt:
./run_final.sh ./test_final.txt ../../particlefilter OCL_particlefilter_double "-x 128 -y 128 -z 10 -np 1000" ./out.txt