include ../../common/make.config

#C compiler
CC = g++

CC_FLAGS = -g -O3 -Wall

EXE = nw

all: nw_cpu nw_gpu

nw_cpu: nw.c
	$(CC) ${KERNEL_DIM} $(CC_FLAGS) -o $@ nw.c -I$(OPENCL_INC) -L$(OPENCL_LIB) -lOpenCL

nw_gpu: nw.c
	$(CC) ${KERNEL_DIM} $(CC_FLAGS) -o $@ nw.c -I$(OPENCL_INC) -L$(OPENCL_LIB) -lOpenCL -DUSE_GPU

#$(EXE): nw.c
#	$(CC) ${KERNEL_DIM} $(CC_FLAGS) -o $(EXE) nw.c -I$(OPENCL_INC) -L$(OPENCL_LIB) -lOpenCL

clean:
	rm -f $(EXE) lud_cpu lud_gpu
