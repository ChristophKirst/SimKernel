/***********************************************************************
   sim_mpi.cpp   -  MPI - routines used by Sim / SimControll

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
************************************************************************/
#include "sim_mpi.h"
#include "mpi.h"

void sim_send_signal(int sig, int it, int to)
{
   SIM_MPI_DEBUG("sim_send_signal sig=" << sig, "it=" <<it <<" to="<< to)

   int comm[2];
   comm[0] = sig; comm[1] = it;
   MPI::COMM_WORLD.Send(comm, 2, MPI::INT, to, SIM_MPI_TAG);
};

void sim_recv_signal(int& sig, int&it, int& from)
{
   SIM_MPI_DEBUG("sim_recv_signal from=", from)

   int comm[2];
   MPI::Status status;
   MPI::COMM_WORLD.Recv(comm, 2, MPI::INT, from, SIM_MPI_TAG, status);
   sig = comm[0];
   it = comm[1];
   from = status.Get_source();

   SIM_MPI_DEBUG("sim_recv_signal recvied: sig="<<sig<<" it="<<it, "from="<<from)
};

void sim_send_message(std::string msg, int to)
{
   SIM_MPI_DEBUG("sim_send_message to=" << to, "msg="<< msg.c_str())

   int length = msg.length();
   MPI::COMM_WORLD.Send(&length, 1, MPI::INT, to, SIM_MPI_TAG);
   MPI::COMM_WORLD.Send(msg.c_str(), length, MPI::CHAR, to, SIM_MPI_TAG);
};

void sim_recv_message(std::string& msg, int& from)
{
   SIM_MPI_DEBUG("sim_recv_message from=", from)

   MPI::Status status;
   int length;
   MPI::COMM_WORLD.Recv(&length, 1, MPI::INT, from, SIM_MPI_TAG, status);
   SIM_MPI_DEBUG("sim_recv_message length="<<length, " from="<<(status.Get_source()))

   char m[length];
   MPI::COMM_WORLD.Recv(m, length, MPI::CHAR, from, SIM_MPI_TAG, status);
   msg = std::string(m, length);
   from = status.Get_source();

   SIM_MPI_DEBUG("sim_recv_message recieved: from="<<from, "msg="<<msg)
};

void sim_send_init(int niter, int to)
{
   SIM_MPI_DEBUG("sim_send_init to="<<to, "niter="<<niter)

   MPI::COMM_WORLD.Send(&niter, 1, MPI::INT, to, SIM_MPI_TAG);
};

void sim_recv_init(int& niter, int from)
{
   MPI::Status status;
   MPI::COMM_WORLD.Recv(&niter, 1, MPI::INT, from, SIM_MPI_TAG, status);

   SIM_MPI_DEBUG("sim_recv_init from="<<from, "niter="<<niter)
}; 

void sim_send_do(int iter, int to)
{
   SIM_MPI_DEBUG("sim_send_do to="<<to, "iter="<<iter)

   MPI::COMM_WORLD.Send(&iter, 1, MPI::INT, to, SIM_MPI_TAG);
};

void sim_recv_do(int& iter, int from)
{
   MPI::Status status;
   MPI::COMM_WORLD.Recv(&iter, 1, MPI::INT, from, SIM_MPI_TAG, status);

   SIM_MPI_DEBUG("sim_recv_do from="<<from, "iter="<<iter)
};

void sim_send_finalize(int rank, int to)
{
   SIM_MPI_DEBUG("sim_send_finalize to="<<to, "rank="<<rank)

   MPI::COMM_WORLD.Send(&rank, 1, MPI::INT, to, SIM_MPI_TAG);
};

void sim_recv_finalize(int& rank, int from)
{
   MPI::Status status;
   MPI::COMM_WORLD.Recv(&rank, 1, MPI::INT, from, SIM_MPI_TAG, status);

   SIM_MPI_DEBUG("sim_recv_finalize from="<<from, "rank="<<rank)
}; 

