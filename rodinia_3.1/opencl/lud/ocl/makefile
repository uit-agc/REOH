include ../../../common/make.config

INC = -I$(OPENCL_INC) -I../common
LIB = -L$(OPENCL_LIB)

#C compiler
CC = g++
CC_FLAGS = -g -O3 -Wall

EXE = lud

all: lud_cpu lud_gpu

lud_cpu: lud.cpp
	$(CC) $(KERNEL_DIM) $(CC_FLAGS) -o $@ lud.cpp ../common/common.c $(INC) $(LIB) -lOpenCL

lud_gpu: lud.cpp
	$(CC) $(KERNEL_DIM) $(CC_FLAGS) -o $@ lud.cpp ../common/common.c $(INC) $(LIB) -lOpenCL -DUSE_GPU

$(EXE): lud.cpp
	$(CC) $(KERNEL_DIM) $(CC_FLAGS) -o $(EXE) lud.cpp ../common/common.c $(INC) $(LIB) -lOpenCL

clean:
	rm -f $(EXE) lud_cpu lud_gpu
