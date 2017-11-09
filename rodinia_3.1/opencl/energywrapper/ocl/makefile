include ../../../common/make.config
PCM = /opt/intel-pcm
NVML = /usr/include/nvidia/gdk
INC_CPU = -I$(OPENCL_INC) -I../common/CPU -I$(PCM) 
INC_GPU = -I$(OPENCL_INC) -I../common/GPU -I$(NVML) 
LIB_CPU = -L$(OPENCL_LIB) -L$(PCM) -lnuma -lOpenCL -lpthread
LIB_GPU = -L$(OPENCL_LIB) -lnvidia-ml -lOpenCL

#C compiler
CC = g++
NVCC = /usr/local/cuda-6.5/bin/nvcc
MeterPU_FLAGS = -g3 -O3 -Wall -std=c++11  -DENABLE_PCM -DENABLE_PTHREAD
MeterPU_GPU_FLAGS = -O3 -gencode arch=compute_35,code=sm_35 -DENABLE_NVML -DENABLE_PTHREAD

main_cpu: main_cpu.cpp 
	$(CC) $(KERNEL_DIM) $(MeterPU_FLAGS) $(PCM)/cpucounters.cpp $(PCM)/client_bw.cpp $(PCM)/msr.cpp $(PCM)/pci.cpp exe_lib_cpu.cpp -DNOBM -o main_cpu main_cpu.cpp $(INC_CPU) $(LIB_CPU)
main_gpu: main_gpu.cpp
	$(NVCC) $(KERNEL_DIM) $(MeterPU_GPU_FLAGS) exe_lib_gpu.cpp -o main_gpu main_gpu.cpp  $(INC_GPU) $(LIB_GPU) 

test_ene_cpu: test_static_ene_cpu.cpp 
	$(CC) $(KERNEL_DIM) $(MeterPU_FLAGS) $(PCM)/cpucounters.cpp $(PCM)/client_bw.cpp $(PCM)/msr.cpp $(PCM)/pci.cpp  -DNOBM -o $@ $^ $(INC_CPU) $(LIB_CPU)

test_ene_gpu: test_static_ene_gpu.cpp
	$(NVCC) $(KERNEL_DIM) $(MeterPU_GPU_FLAGS) -o $@ $^  $(INC_GPU) $(LIB_GPU) 


#use MeterPU_FLAGS
#-lnuma for numa 
#-lpthread -L$(PCM) for meterpu
#-lOpenCL for rodinia
#added MeterPU.h to ../common/CPU and meterpu.h to ../common/GPU
clean:
	rm -f main_cpu
	rm -f main_gpu

#USAGE:
#make clean
#make main_cpu
#before make with nvcc, export lib and path: 
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda-6.5/lib64
#export PATH=$PATH:/usr/local/cuda-6.5/bin

#g++ -c /opt/intel-pcm/client_bw.cpp
library_cpu: $(PCM)/client_bw.cpp $(PCM)/cpucounters.cpp $(PCM)/msr.cpp $(PCM)/pci.cpp exe_lib_cpu.cpp
	$(CC) -c $(MeterPU_FLAGS) $(PCM)/client_bw.cpp $(INC_CPU)
	$(CC) -c $(MeterPU_FLAGS) $(PCM)/cpucounters.cpp $(INC_CPU)
	$(CC) -c $(MeterPU_FLAGS) $(PCM)/msr.cpp $(INC_CPU)
	$(CC) -c $(MeterPU_FLAGS) $(PCM)/pci.cpp $(INC_CPU)
	$(CC) -c $(MeterPU_FLAGS) exe_lib_cpu.cpp $(INC_CPU)
	ar rcs lib_cpu.a cpucounters.o client_bw.o msr.o pci.o exe_lib_cpu.o 
	g++ -DNOBM -o main_cpu main_cpu.cpp lib_cpu.a $(INC_CPU) $(LIB_CPU)

library_gpu: exe_lib_gpu.cpp
	$(NVCC) -c $(MeterPU_GPU_FLAGS) exe_lib_gpu.cpp $(INC_GPU) $(LIB_GPU)
	ar rcs lib_gpu.a exe_lib_gpu.o 
	g++ -DNOBM -o main_gpu main_gpu.cpp lib_gpu.a $(INC_GPU) $(LIB_GPU) 
