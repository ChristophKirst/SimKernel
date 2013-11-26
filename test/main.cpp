/***************************************************************************
   main.cpp   -  example main program 
   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/

#include <iostream>                           // cout, cerr
#include <string>
#include <sstream>
using namespace std;

#define SIM_PRE "Sim: "
#include "sim_main.h"




class Kernel
{
public:

   double a,b,c;
   
   void initialize(Sim& sim)
   {
   };

   void execute(Sim& sim)
   {
      sim.get("a", a);
      sim.io << "a = " << a << Endl;
      sim.get("b", b);
      sim.io << "b = " << b << Endl;
      
      sim.get("c", c);      
      sim.io << "c = " << c << Endl;     
      
      
      // waste some time
      double k;
      for (int i=0; i< 10000; i++) {
        for (int j =0; j<10000; j++) {
            k = k + i;
        }
      }
      
      double res = a + b;
      
      sim.io << "a + b = " << a << " + " << b << " = " << a +b << Endl;

   };


   void finalize(Sim& sim)
   {
      sim.io << "Kernel (iteration=" << sim.iteration() << "/" << sim.n_iterations() << ") done!" << Endl;
   };
};


int main(int argc, char* argv[])
{
   SimControl<Kernel> simc;
   //int ret = simc.simulate(argc, argv);

   int ret = simc.simulate(argc, argv);

   return 0;
};

