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


class TestKernel
{
public:
   // interface to Sim needs three functions:
   //void initialize(Sim& sim)
   //void execute(Sim& sim)
   //void finalize(Sim& sim)

   void initialize(Sim& sim)
   {
      //cout << "TestKernel: initialize()" << endl;

      /*
      cout << "testing messaging ...." << endl;
      sim.io << "This is a message to the sim environment!" << Msg;

      //cout << "testing errors ...." << endl;
      sim.io << "This is an warning to the sim environment!" << Warning;
 //     sim.io << "This is an error to the sim environment!" << Abort;
 //     sim.io << "This is an error to the sim environment!" << Exit;

      //cout << "testing the sim expression kernel" << endl;

      sim.io << "This is iteration: " << sim.iteration() << " and process " << sim.process();
      sim.io << Endl;

      double par1;
      sim.get("a", par1, 100);

      sim.define("defined", 1000);

      sim.get("defined", par1, 100);
      std::cout << " defined = " << par1 << endl;

      Sim::expr_t expr;
      sim.get_definition("par1", expr);
      sim.io << "par1 is defined as: " << expr << Warning;


      sim.get("notapar", par1, 100.0, Warning);

      sim.get("notapar", par1, 100.0, Message);

      sim.get("par1", par1, 10.4);

      std::string str;
      sim.get("par1", str, "No String!");
//      cout << "par1 = " << str << endl;

      vector<int> v;
      sim.get_list("list", v, -500);
      // or 
      sim.get("list", v, -500);   

      sim.io << "v has size:" << v.size();
      for (int i = 0; i < v.size(); i++)
         sim.io << " v[" <<i <<"]=" << v[i];
      sim.io << Endl;

      vector< vector<int> > m;
      sim.get_matrix("matrix", m, -100);
      // or
      sim.get_matrix("matrix", m, -100);

      sim.io << "m has size:" << m.size();
      for (int i = 0; i < m.size(); i++)
         for (int j = 0; j < m[i].size(); j++)
         sim.io << " m[" << i <<"][" << j <<"]=" << m[i][j];
      sim.io << Endl;

      if (sim.iteration()==40) sim.io << "Lets exit!" << Exit;

      */

   };

   void execute(Sim& sim)
   {
//       sim.io << "TestKernel: execute(Sim)" << Endl;

/*
      if (sim.iteration() == 2)
         sim.io << "TestKernel: Abort iteration:" << sim.iteration() << "!" << Abort;

      // testing Creator
      if (sim.iteration() == sim.n_iterations())
      {
         cout << sim.info();
         Sim::expr_t e;
         sim.get_definition("c", e);
         sim.io << "c is deifned as: " << e << Endl;
         double cre;
         for (int i = 0; i < 15; i++)
         {
            sim.get("c", cre, -800.);
            sim.io << "Creator access i="<<i<< " c=" <<cre << Endl;
         }; 
      };

      */
      
      // waste some time
      double k;
      for (int i=0; i< 10000; i++) {
        for (int j =0; j<100000; j++) {
            k = k + i;
        }
      }
      

   };


   void finalize(Sim& sim)
   {
      /*
      Sim::expr_t par;
      sim.get_expr("params", par);
      sim.io << "params = " << par << Endl;

      */
      std::vector<double> vd;
      sim.get("l", vd, 0.0);

      Sim::expr_t p;
      sim.get("l", p);

      sim.io << "l =" << p << Endl;

      sim.io << "lenght of l =" << vd.size() << Endl;

      sim.io << "TestKernel: finalize(Sim)" << Endl;

      std::string str;
      sim.get("str", str, "");
      sim.io << "str=" << str << Msg;


      sim.io << "End of TestKernel (iteration=" << (sim.iteration()) << ")" << Endl;

      sim.io << "hallo" << sim.iteration() << " " << sim.n_iterations() << Endl;

   };
};


int main(int argc, char* argv[])
{
   SimControl<TestKernel> simc;
   int ret = simc.simulate(argc, argv);

   ret = simc.simulate_online(argc, argv);

   return 0;
};

