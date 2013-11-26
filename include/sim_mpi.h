/***********************************************************************
   sim_mpi.h   -  MPI - routines used by Sim / SimControll

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
************************************************************************/
#ifndef SIM_MPI_H
#define SIM_MPI_H

#include <string>
#include "mpi.h"

#define SIM_MPI_CNTRL 0
#define SIM_MPI_TAG 100

//#include <iostream>
//#define SIM_MPI_DEBUG(s1,s2) std::cout << "SimMpi Debug: " << s1 << " " << s2 << std::endl; std::cout.flush();
#define SIM_MPI_DEBUG(s1,s2)

void sim_send_signal(int sig, int iter, int to /* = SIM_MPI_CNTRL*/);
void sim_recv_signal(int& sig, int&iter, int& from);

void sim_send_message(std::string msg, int to /*= SIM_MPI_CNTRL*/);
void sim_recv_message(std::string& msg, int& from);

void sim_send_init(int niter, int to);
void sim_recv_init(int& niter, int from /*= SIM_MPI_CNTRL*/);

void sim_send_do(int iter, int to);
void sim_recv_do(int& iter, int from /*= SIM_MPI_CNTRL*/);

void sim_send_finalize(int rank, int to /*= SIM_MPI_CNTRL*/);
void sim_recv_finalize(int& rank, int from /*= MPI::ANY_SOURCE*/);

#endif

