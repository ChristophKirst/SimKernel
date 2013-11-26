/***********************************************************************

   sim_main.cpp   - Include file for main.cpp with user kernel:

                     #include "Kernel.h"
                     #include "sim_main.h"

                     int main(int argc, char* argv[])
                     {
                        SimControl<Kernel> simc;
                        return simc.simulate(argc, argv);
                     };


   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
************************************************************************/
#ifndef SIM_MPI
#include "sim_control.h"
#else
#include "sim_control_mpi.h"
#endif

