include ../../common/make.config

#C compiler
CC = g++


FLAGS = -g
SRC = streamcluster.cpp
EXE = streamcluster

all: sc_cpu sc_gpu

sc_cpu:$(SRC)
	$(CC) -lOpenCL  $(SRC) -o $@ -I$(OPENCL_INC) -L$(OPENCL_LIB)	$(FLAGS)

sc_gpu:$(SRC)
	$(CC) -lOpenCL  $(SRC) -o $@ -I$(OPENCL_INC) -L$(OPENCL_LIB)	$(FLAGS) -DUSE_GPU

release:$(SRC)
	$(CC) -lOpenCL  $(SRC) -o $(EXE) -I$(OPENCL_INC) -L$(OPENCL_LIB)	$(FLAGS)

errmsg:$(SRC)
	$(CC) -lOpenCL $(SRC) -o $(EXE) -I$(OPENCL_INC) -L$(OPENCL_LIB) -D ERRMSG $(FLAGS)

ptx:$(SRC)
	$(CC) -lOpenCL $(SRC) -o $(EXE) -I$(OPENCL_INC) -L$(OPENCL_LIB) -D PTX_MSG $(FLAGS)

res:$(SRC)
	$(CC) -lOpenCL $(SRC) -o $(EXE) -I$(OPENCL_INC) -L$(OPENCL_LIB) -D RES_MSG $(FLAGS)

clean: $(SRC)
	rm -f $(EXE) $(EXE).linkinfo result* sc_cpu sc_gpu
