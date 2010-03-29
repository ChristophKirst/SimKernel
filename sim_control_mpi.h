/***********************************************************************
   simcontrol_mpi.h   -   Simulation Flow Control MPI parallelized

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
************************************************************************/
#ifndef SIM_CONTROL_MPI_H
#define SIM_CONTROL_MPI_H

#include <sstream>
#include "mpi.h"

#include "expression_parser.h"
#include "sim_io_manager.h"
#include "sim_signals.h"
#include "sim_mpi.h"
#include "sim.h"

//#define SCM_DEBUG(s1, s2) std::cout << "SimControll Parallel DEBUG: " << s1 << " " << s2 << std::endl;
#define SCM_DEBUG(s1,s2)

#ifndef SIM_PRE
#define SIM_PRE "Sim: "
#endif


template<typename KernelT>
class SimControl
{
private:
   // parameter
   Sim               sim;

   // parser
   ExprParser        parser;

   // mpi
   int mpi_rank, mpi_size;
   MPI::Status mpi_status;

public:

   SimControl() : sim() {};

   int simulate(int argc, char* argv[])
   {
      MPI::Init(argc, argv);
      mpi_rank = MPI::COMM_WORLD.Get_rank();
      mpi_size = MPI::COMM_WORLD.Get_size();

      if (mpi_rank == SIM_MPI_CNTRL)  // process manager
      {
         SCM_DEBUG("Manager: simulate() ", mpi_rank) 

         // io
         SimIOManager io;
         io.init(true, SIM_PRE, true, false, "sim.log");

         // check init
         std::string err;
         bool ready = (mpi_size > 1);
         if (!ready) err = "Parallel version requires at least two processes!";

         if (ready) 
         {
            ready = !(argc<2);
            if (!ready) err = "No input file specified!";
         };

         if (ready) ready = parser.parse(argv[1], err);
         SCM_DEBUG("Manager: parser.result=", parser.result)

         if (ready) ready = sim.init(parser.result, err);
         int n_iterations = sim.n_iterations();

         if (!ready)
         {
            for (int p = 1; p < mpi_size; p++)
               sim_send_signal(Exit, -1, p);
            io.message(err, Exit);
         } 
         else // if (ready)
         {
            io.set_iterations(n_iterations);

            for (int p = 1; p < mpi_size; p++)
            {
               sim_send_signal(Success, -1, p);
               sim_send_init(n_iterations, p); // initialize working processes
            };
         } 

         int src, it, sig;
         int done_procs = 0;
         int iter = -1;
         bool errs = false;

         while (ready)
         {
            SCM_DEBUG("Manager: waiting for signal! iter=", iter)

            // wait for next signal
            src = MPI::ANY_SOURCE;
            sim_recv_signal(sig, it, src);
            SCM_DEBUG("Manager: recieved signal (src,it,sig)=(", sig <<","<<it<<","<<src<<")")
            SCM_DEBUG("Manager: sig="<<sig, "is_io_signal="<<(is_io_signal(sig)))

            if (is_io_signal(sig)) // message or error
            {
               // recieve a message
               std::string str;
               sim_recv_message(str, src);
               SCM_DEBUG("Manager: Message form: "<<src<<" recived: ", str)

               io.message(str, SimSignal(sig), it, src);

               // what type of message do we have:
               switch (sig)
               {
                  case int(Message):
                  case int(Warning):
                     break;
                  case int(Abort):
                     sig = int(Success);  // to next iteration 
                     errs = true;
                     break;
                  case int(Exit):
                  default:
                     iter = n_iterations;
                     sig = int(Success);  // propagate iter to abort process
                     errs = true;
               };
            }
            if (is_io_output_signal(sig))
            {
               io.set_output_level(SimSignal(sig));
            };
            if (sig == int(Invalid))  // switch off the invalid process
            {
               std::string str;
               sim_recv_message(str, src);
               SCM_DEBUG("Manager: Message form: "<<src<<" it=" << it <<" recived:", str)
               io.message(str, Abort);
               done_procs++;
               errs = true;
            }

            if (sig == int(Success))
            {
               SCM_DEBUG("Manager: Process: "<<src <<" job "<<it<<" done! iter=", iter)

               iter++;
               // start next iteration
               if (iter < n_iterations) 
               {
                  SCM_DEBUG("Manager: Process: "<<src <<" new job:", iter)
                  sim_send_do(iter, src);
               }
               else 
               {
                  SCM_DEBUG("Manager: Process: "<<src <<" done! total done=", done_procs)
                  done_procs++;
               };
               if (done_procs == mpi_size-1) break;
            };
         };  // while (ready)

         SCM_DEBUG("Manager: all jobs done! done_procs=", done_procs)

         // tell all processes to finalize
         for (int p = 1; p < mpi_size; p++)
            sim_send_do(n_iterations, p);

         int final_procs = 0;
         while (final_procs < mpi_size -1)
         {
            sim_recv_finalize(src, MPI::ANY_SOURCE);
            SCM_DEBUG("Manager: Process:"<<src<<" is finished! final_procs=", final_procs)
            final_procs++;
         };

         if (ready && !errs)
         {
            io.message("Simulation done!\n");
            io.error_summary();
            io.message("Bye!");
         }
         if (ready && errs)
         {
            io.message("Simulation done!");
            io.error_summary();
            io.message("Errors in the simulation!");
         }

         io.close();

         SCM_DEBUG("Manager:", "is finished!")
      }
      else  // working processes
      {
         SCM_DEBUG("Proc:"<<mpi_rank, "simulate()")

         // init
         int next_iter = 0;
         int iter = -1;
         int n_iterations = 0;
         int sig, it;
         int from = SIM_MPI_CNTRL;

         sim_recv_signal(sig, it, from);
         bool ready = (sig == int(Success));

         if (ready) sim_recv_init(n_iterations, SIM_MPI_CNTRL);
         ready = (n_iterations > 0) && !(argc<2);

         SCM_DEBUG("Proc:"<<mpi_rank<<" init: n_iters=", n_iterations)

         // initialization for manager process ok try if it works for this process


         std::string err;
         if (ready) ready = parser.parse(argv[1], err);
         if (ready) ready = sim.init(parser.result, err, mpi_rank);

         if (ready)
         {
            ready = (n_iterations == sim.n_iterations());
            if (!ready) err =  "Number of iterations dont match!";
         };

         if(!ready)
         {
            if (sig == int(Success)) // send invalid mesage if init of root pocess was succesful
            {
               std::ostringstream str;
               str << "Process " << mpi_rank << " invalid: " << err;
               sim_send_signal(Invalid, -1, SIM_MPI_CNTRL);
               sim_send_message(str.str(), SIM_MPI_CNTRL);
            };
         } 
         else
         {
            sim_send_signal(Success, -1, SIM_MPI_CNTRL); // this process is ready to do work
         }

         bool succ = true;
         while (true)
         {
            SCM_DEBUG("Proc:"<<mpi_rank<<" iter="<<iter, "waiting for next_iter!")

            sim_recv_do(next_iter, SIM_MPI_CNTRL);
            SCM_DEBUG("Proc:"<<mpi_rank<<" iter="<<iter<<" recieved next_iter=", next_iter)

            if (next_iter >= n_iterations) break; // exit condition

            while (iter < next_iter)
            {
               if (!sim.next_iteration()) 
               {
                  std::stringstream str;
                  str << "Process " << mpi_rank << " at it: " << iter;
                  str << "cannot iterate to next iteration: " << next_iter <<"!";
                  sim_send_signal(Invalid, -1, SIM_MPI_CNTRL);
                  sim_send_message(str.str(), SIM_MPI_CNTRL);
                  succ = false;
               };
               iter++;

               SCM_DEBUG("Proc:"<<mpi_rank<<" iter="<<iter<<" sim.iteration=", sim.iteration())
            };

            // start simulation at next iteration
            SCM_DEBUG("Proc:"<<mpi_rank<<" iter="<<iter, "starting simulation!")

            try {
               KernelT kernel;

               kernel.initialize(sim);
               kernel.execute(sim);
               kernel.finalize(sim);
            }
            catch (SimSignal& sig)
            {
               SCM_DEBUG("Proc:"<<mpi_rank<<" iter="<<iter, "error during sim="<<sig)
               // error message is send by sim.signal() already !
               succ = false;
            };

            SCM_DEBUG("Proc:"<<mpi_rank<<" iter="<<iter, "simulation done!")
            if (succ) sim_send_signal(Success, iter, SIM_MPI_CNTRL);
            else succ = true;
         };


         SCM_DEBUG("Proc:"<<mpi_rank<<" iter="<<iter, "end iter loop!")

         // tell root this process is done!
         sim_send_finalize(mpi_rank, SIM_MPI_CNTRL);

      }; // end manager - wroking process

      MPI::Finalize();
      return 0;
   };

};

#endif
