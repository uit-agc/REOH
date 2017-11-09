include ../../common/make.config
#OPENCL_DIR = /af21/den4gr/NVIDIA_GPU_Computing_SDK/

# C compiler
CC = g++
CC_FLAGS = -O2 
#INCLUDES = -I$(OPENCL_DIR)/OpenCL/common/inc -I$(OPENCL_DIR)/shared/inc/
INCLUDES = -I$(OPENCL_INC)

#ALL: OCL_particlefilter_naive OCL_particlefilter_double OCL_particlefilter_single

ALL: OCL_particlefilter_single_cpu OCL_particlefilter_single_gpu OCL_particlefilter_double_cpu OCL_particlefilter_double_gpu

OCL_particlefilter_naive: ex_particle_OCL_naive_seq.o
	$(CC) $(CC_FLAGS) ex_particle_OCL_naive_seq.o $(INCLUDES) -L$(OPENCL_LIB) -lOpenCL -o OCL_particlefilter_naive 

OCL_particlefilter_double_cpu: ex_particle_OCL_double_seq_cpu.o
	$(CC) $(CC_FLAGS) ex_particle_OCL_double_seq_cpu.o $(INCLUDES)  -L$(OPENCL_LIB) -lOpenCL -o $@

OCL_particlefilter_double_gpu: ex_particle_OCL_double_seq_gpu.o
	$(CC) $(CC_FLAGS) ex_particle_OCL_double_seq_gpu.o $(INCLUDES)  -L$(OPENCL_LIB) -lOpenCL -o $@

OCL_particlefilter_single_cpu: ex_particle_OCL_single_seq_cpu.o
	$(CC) $(CC_FLAGS) ex_particle_OCL_single_seq_cpu.o $(INCLUDES)  -L$(OPENCL_LIB) -lOpenCL -o $@

OCL_particlefilter_single_gpu: ex_particle_OCL_single_seq_gpu.o
	$(CC) $(CC_FLAGS) ex_particle_OCL_single_seq_gpu.o $(INCLUDES)  -L$(OPENCL_LIB) -lOpenCL -o $@


%.o: %.[ch]
	$(CC) $(CC_FLAGS) $< -c

ex_particle_OCL_naive_seq.o: ex_particle_OCL_naive_seq.cpp
	$(CC) $(CC_FLAGS) $(INCLUDES) ex_particle_OCL_naive_seq.cpp -c

ex_particle_OCL_double_seq_cpu.o: ex_particle_OCL_double_seq.cpp
	$(CC) $(CC_FLAGS) $(INCLUDES) ex_particle_OCL_double_seq.cpp -c -o $@

ex_particle_OCL_double_seq_gpu.o: ex_particle_OCL_double_seq.cpp
	$(CC) $(CC_FLAGS) $(INCLUDES) ex_particle_OCL_double_seq.cpp -c -o $@ -DUSE_GPU

ex_particle_OCL_single_seq_cpu.o: ex_particle_OCL_single_seq.cpp
	$(CC) $(CC_FLAGS) $(INCLUDES) ex_particle_OCL_single_seq.cpp -c -o $@

ex_particle_OCL_single_seq_gpu.o: ex_particle_OCL_single_seq.cpp
	$(CC) $(CC_FLAGS) $(INCLUDES) ex_particle_OCL_single_seq.cpp -c -o $@ -DUSE_GPU

clean:
	rm -f *.o *~ OCL_particlefilter_naive OCL_particlefilter_double OCL_particlefilter_single *.linkinfo OCL_particlefilter_single_gpu OCL_particlefilter_single_cpu OCL_particlefilter_double_cpu OCL_particlefilter_double_gpu
