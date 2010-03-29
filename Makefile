# ----------------------------------- #
# Makefile for Sim                    #
# ----------------------------------- #

# single or parallel
MPI = off
EXE = test

ifeq ($(MPI), on)
CC     = mpic++
CFLAGS = -DSIM_MPI
else
CC     = g++
CFLAGS = 
# -O2 
endif

LDFLAGS  = -lgsl -L.
OBJ  = main.o expression.o expression_basic.o expression_sim.o expression_extra.o expression_parser.o sim_io_manager.o sim_io.o sim_signals.o sim.o

EXPRH = expression.h expression_basic.h expression_sim.h expression_extra.h expression_parser.h
SIMH =  sim.h sim_io.h sim_signals.h sim_control.h sim_io_manager.h 

ifeq ($(MPI), on)
OBJ = sim_mpi.o $(OBJ)
SIMH = sim_mpi.h sim_control_mpi.h  $(SIMH)
endif


%.o : %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

all : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXE) $(LDFLAGS)

# Dependencies

main.o : main.cpp $(EXPRH) $(SIMH)
sim.o : sim.cpp sim.hpp $(SIMH)
sim_io.o : sim_io.cpp sim_io.h 
sim_mpi.o : sim_mpi.cpp sim_mpi.h
sim_signals.o : sim_signals.cpp sim_signals.h
sim_io_manager.o : sim_io_manager.cpp sim_io_manager.h sim_signals.h

expression.o : expression.cpp expression.h
expression_basic.o : expression_basic.cpp expression_basic.h expression.h
expression_sim.o : expression_sim.cpp expression_sim.h expression.h expression_basic.h 
expression_extra.o : expression_extra.cpp expression_extra.h expression.h expression_basic.h 
expression_parser.o : expression_parser.cpp expression_parser.h expression.h expression_basic.h expression_sim.h expression_extra.h


clean:
	rm -f test *.o *~

