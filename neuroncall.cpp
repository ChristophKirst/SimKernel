/***************************************************************************
   neuroncall.cpp   -  for parameter scans with Neuron
   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/

#include <iostream>                           // cout, cerr
#include <string>
#include <sstream>
using namespace std;

#define SIM_PRE "NeuronSim: "
//#define SIM_MPI
#include "sim_main.h"

class NeuronCall
{
public:
   std::string command;

   void initialize(Sim& sim)
   {
      std::string hocfile, nrncmd;
      sim.get("hoc", hocfile, "start.hoc");
      sim.get("nrn", nrncmd, "nrniv");
      long it = sim.iteration();
      std::stringstream str;
      str << it;
      command = nrncmd + " -c \"iteration = " + str.str() + "\" " + hocfile;

      sim.io << "Neuron Command: " << command << Msg;
      system(command.c_str());
   };

   void execute(Sim& sim)
   {
   };

   void finalize(Sim& sim)
   {
      sim.io << "Neuron Command: " << command << " done !" << Msg;
   };
};


int main(int argc, char* argv[])
{
   SimControl<NeuronCall> simc;
   int ret = simc.simulate(argc, argv);
   return 0;
};

