* run.sh:

Test environment:
- GNU bash, version 4.2.46(2)-release (x86_64-redhat-linux-gnu)
- GNU Awk 4.0.2
- Linux 3.10.0

Note: These are proof-of-concept scripts glued together into one script (run.sh)

SYNOPSIS: run.sh  <app_exe_loc> <app_exe_basename> <app_arg> <bench_name>

Example:
$ ./run.sh rodinia_3.1/opencl/particlefilter/ OCL_particlefilter_double "-x 128 -y 128 -z 2500 -np 10000" pf_128_2500_1000
$ ./run.sh rodinia_3.1/opencl/particlefilter/ OCL_particlefilter_double "-x 128 -y 128 -z 10 -np 1000" pf_128_10_100
$ ./run.sh rodinia_3.1/opencl/kmeans/ kmeans "-i ../../data/kmeans/inpuGen/10000_34.txt" kmeans_10000_34
$ ./run.sh rodinia_3.1/opencl/lud/ocl/ lud "-s4096" lud_4096
$ ./run.sh rodinia_3.1/opencl/bfs/ bfs "../../data/bfs/inputGen/graph512k.txt" bfs_512
$ ./run.sh rodinia_3.1/opencl/cfd/ cfd "../../data/cfd/fvcorr.domn.097K" cfd_fvcorr_097k

Prerequisites:
1. Training data
 - Samples can be found in ./Data/train
2. Two versions of an executable with postfix {_cpu, _gpu}
 - leo_fmt.sh can be used to format the training data for leo (EM framework).
2. ./ConfigList.txt must correspond to X.txt in training data.
 - Sample ConfigList.txt included
3. Static energy measurements are hardcoded into leo_fmt.sh and run_test.sh

Description:
 - Configurations for testing phases can be modified in run_test.sh

run.sh will based on a sample configuration run a test for a specific application that has a gpu and a cpu executable. The outputs of this test will be stored in ./Data/test. Next it will run the EM framework on the output of the test phase. Finally it will take the configuration given by the EM framework and run the best configuration