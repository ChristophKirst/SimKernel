# ----------------------------------- #
# Makefile for the SimKernel          #
# ----------------------------------- #
# by
# Christoph Kirst 
# Max Planck Institute for Dynamics and Self-Organization
# christoph@nld.ds.mpg.de 
  

# sim file settings
LIB = lib/sim
LIB_MPI = $(LIB)_mpi

EXE = test/test
EXE_MPI = $(EXE)_mpi
EXE_SRC = test/main.cpp

# CPP_FILES = $(wildcard src/*.cpp)
CPP_FILES = sim.cpp sim_io.cpp sim_signals.cpp sim_io_manager.cpp expression.cpp expression_basic.cpp expression_sim.cpp expression_extra.cpp expression_parser.cpp
SRC_FILES = $(addprefix src/,$(CPP_FILES))
SRC_FILES_MPI = $(SRC_FILES) src/sim_mpi.cpp

# compilation settings
CC     = g++
CFLAGS = -I./include -O3
CC_MPI     = mpic++
CFLAGS_MPI = -DSIM_MPI -I./include -O3

LDFLAGS  = -lgsl -lgslcblas -L.
LDFLAGS_MPI  = $(LDFLAGS)

AR=ar
ARFLAGS=rcs


OS := $(shell uname)
ifeq ($(OS),Linux)
LDFLAGS  += -lrt
LDFLAGS_MPI += -lrt
endif




# make
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRC_FILES))
OBJS_MPI = $(patsubst src/%.cpp,mpi/%.o,$(SRC_FILES_MPI))

EXE_OBJ = $(patsubst %.cpp,%.o,$(EXE_SRC))
EXE_OBJ_MPI = $(patsubst %.cpp, %_mpi.o,$(EXE_SRC))

DEPS = $(patsubst src/%.cpp,obj/%.d,$(SRC_FILES))
DEPS_MPI = $(patsubst src/%.cpp,mpi/%.d,$(SRC_FILES_MPI))

all : lib lib_mpi exe exe_mpi

exe : $(OBJS) $(EXE_OBJ)
	$(CC) $(CFLAGS) $(OBJS) $(EXE_OBJ) -o $(EXE)  $(LDFLAGS) 

exe_mpi : $(OBJS_MPI) $(EXE_OBJ_MPI)
	$(CC_MPI) $(CFLAGS_MPI) $(OBJS_MPI) $(EXE_OBJ_MPI) -o $(EXE_MPI) $(LDFLAGS_MPI)

lib :  $(OBJS)
	$(AR) $(ARFLAGS) $(LIB).a $(OBJS)

lib_mpi : $(OBJS_MPI)
	$(AR) $(ARFLAGS) $(LIB_MPI).a $(OBJS_MPI)

test : exe

test_mpi : exe_mpi

$(EXE_OBJ) : $(EXE_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(EXE_OBJ_MPI) : $(EXE_SRC)
	$(CC_MPI) $(CFLAGS_MPI) -c -o $@ $<
	

obj/%.o : src/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

mpi/%.o : src/%.cpp
	$(CC_MPI) $(CFLAGS_MPI) -c -o $@ $<	

	
.PHONY : clean
clean :
	rm -f $(OBJS) $(OBJS_MPI) $(EXE_OBJ) $(EXE_OBJ_MPI) $(DEPS) $(DEPS_MPI)
	rm -f *~ src/*~ $(TEST) $(TEST_MPI) $(LIB).a $(LIB_MPI).a

tutorial_sim_iterated: $(OBJBASE) tutorial/hello_world_sim_iterated.cpp
	$(CC) $(CFLAGS) tutorial/hello_world_sim_iterated.cpp $(OBJBASE) -o tutorial_sim_iterated $(LDFLAGS)

# Auto genereate Dependencies
CFLAGS += -MD
CFLAGS_MPI += -MD

-include $(DEPS)
-include $(DEPS_MPI)

# DO NOT DELETE
