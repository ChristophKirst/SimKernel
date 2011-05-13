/***********************************************************************
   sim_control.h   -   Simulation Flow Control

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
************************************************************************/
#ifndef SIM_CONTROL_H
#define SIM_CONTROL_H

#include <sstream>
#include <string.h>

#include "expression_parser.h"
#include "sim_io_manager.h"
#include "sim.h"


#ifndef SIM_PRE
#define SIM_PRE "Sim: "
#endif


template<typename KernelT>
class SimControl
{
private:
    // io
   SimIOManager      io;

   // parameter
   Sim               sim;

   // parser
   ExprParser        parser;

public:

   SimControl() {};

   int simulate(int argc, char* argv[])
   {

      io.init(true, SIM_PRE, true, false, "sim.log");
      sim.init_io(&io);

      if (argc<2) 
      {
         io.message("No input file specified!", Exit);
         io.close();
         return int(Exit);
      };

      std::string err;
      if (!parser.parse_file(argv[1], err))
      {
         io.message(err);
         io.message("Simulation initialization failed!", Exit);
         io.close();
         return int(Exit);
      };

      if (!sim.init(parser.result, err))
      {
         io.message(err);
         io.message("Simulation initialization failed!", Exit);
         io.close();
         return int(Exit);
      };

      io.set_iterations(sim.n_iterations());

      long itstart = 0;
      long itend = sim.n_iterations();
      if (argc==3) 
      {
         if (strcmp(argv[2], "iterations")==0) {  // return number of iterations to stdout
            std::cout << sim.n_iterations() << std::endl;
            return int(Success);
         } else {
            std::stringstream str(argv[2]);
            str >> itstart;
            itend = itstart;
         }
      }
      if (argc==4) 
      {
         std::stringstream strs(argv[2]);
         strs >> itstart;
         std::stringstream stre(argv[3]);
         stre >> itend;
         if (itend<0) itend = sim.n_iterations()+1+itend;
      }


      SimSignal reterr = Success;

      while (sim.next_iteration())
      {
         if (itstart <= sim.iteration() && sim.iteration() <= itend) {
            try {
               std::stringstream str;
               str << "Starting Simulation iteration: " << (sim.iteration());
               str << "/" << sim.n_iterations() << std::endl;
               io.message(str.str());
      
               KernelT kernel;
      
               kernel.initialize(sim);
               kernel.execute(sim);
               kernel.finalize(sim);

               std::stringstream str2;
               str2 << "Simulation iteration: " << (sim.iteration());
               str2 << "/" << sim.n_iterations() << " done!" << std::endl;
               io.message(str2.str());
            }
            catch (const SimSignal& e)
            {
               SIM_DEBUG("catch sig=", e)
               switch (e)
               {
                  case Abort:
                     io.message("Simulation run aborted due to error!");
                     reterr = Abort;
                     break;
                  case Exit:
                  default:
                     io.message("Simulation aborted due to fatal error!");
                     io.summary();
                     io.close();
                     return int(Exit);
               }
            }
         } // check for iteration range 
      } // while

      io.message("Simulation done!\n");
      io.error_summary();
      io.message("Bye!");
      io.close();

      return int(reterr);
   };

   int simulate_online(int argc, char* argv[])
   {
      int in = 1;

      io.init(true, SIM_PRE, true, false, "sim.log");
      sim.init_io(&io);

      std::string err;
      ExprPtrT g = ExprPtrT(new ExprGlobal());
      if (!sim.init(g, err))
      {
         io.message(err);
         io.message("Sim initialization failed!", Exit);
         io.close();
         return int(Exit);
      };

      std::string cmd;
      err = "";
      cout << "In["<<in<<"]:";
      //cin >> cmd;
      getline(cin, cmd);
      while (cmd != "Quit[]" && cmd != "q")
      {
         if (!parser.parse(cmd, err))
         {
            io.message("Cannot parse command: " + cmd);
            io.message(err);
            err = "";
         } else {
            ExprPtrT res, resprt;
            //cout << parser.result << std::endl;
            
            resprt = new ExprBlock();
            resprt->arg = parser.result->arg;
            cout << "Parse[" <<in <<"]:" << resprt << std::endl;           
 
            cout << "Out["<<in<<"]:"; // for intermediate ouputs such as Print etc
            try {
               ExprArgT::iterator it = resprt->arg.begin();
               while (it != resprt->arg.end() )
               {
                  res = it->get()->evaluate(&sim.scope);
                  it++;
               };
               // make global header to block
               //cout << parser.result << std::endl;
               //res = resprt->evaluate(&sim.scope);
            }
            catch (const SimSignal& e)
            {
               io.message("evaluation error!");
               switch (e)
               {
                  case Abort:
                  case Exit:
                     cmd = "q"; break;
                  default:
                     cmd = ""; break;
               }
            }
            cout << std::endl << "Result["<<in<<"]:" << res << std::endl;
         }
         in++;
         cout << "In["<<in<<"]:";
         getline(cin, cmd);
      }; // while 

      io.message("Bye!");
      io.close();

      return 0;
   };


};

#endif
