/***************************************************************************
   sim.cpp -   interface between parameter expressions 
               and simulation kernel

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#include "sim.h"
#include "sim_signals.h"

#ifdef SIM_MPI
#include "sim_mpi.h"
#endif

Sim::Sim() :  io(this), err(this), msg(this), global(ExprNullPtr()), proc(-1), iter(-1), n_iters(0)
{
   SIM_DEBUG("Sim::creator() global=", global<< " proc =" << proc << " global.ptr=" << global.ptr )
};

Sim::Sim(const ExprPtrT& g) : io(this), err(this), msg(this), global(g), proc(-1), iter(-1), n_iters(0)
{
};

void Sim::clear()
{
   scope.clear();
   global->clear();
   proc = -1;
   iter = -1; 
   n_iters = 0;
};

bool Sim::init(const ExprPtrT& g, std::string& err, int p)
{
   SIM_DEBUG("Sim::init() g=", g)
   SIM_DEBUG("Sim::init() p=", p)
   SIM_DEBUG("Sim::init() proc=", proc)
   SIM_DEBUG("Sim::init() global=", global)

   global = g;
   proc = p;
   iter = 0;

   try 
   {
      global->evaluate(&scope);
      n_iters = ExprIterator::count_iterations(&scope);
   }
   catch (const ExpressionError& exprerr)
   {
      SIM_DEBUG("Sim::init() error while evaluating global:", exprerr.what())
      err = std::string("Parameters could not be initialized: " + exprerr.what());
      return false;
   };

   SIM_DEBUG("Sim::init() sucess! n_iters=", n_iters)
   SIM_DEBUG("Sim::init() sucess! scope=", scope.info())
   return true;
};

#ifndef SIM_MPI
void Sim::init_io(SimIOManager* iomngr_)
{
   iomngr = iomngr_;
};
#endif


bool Sim::next_iteration()
{
   iter++;
   io.release(); err.release(); msg.release();
   try
   {
      return ExprIterator::next_iteration(&scope);
   } 
   catch (const ExpressionError& exprerr)
   {
      iter = -1;
      io << "Expression error while increasing iteration: " << exprerr.what() << Exit;
      return false;
   };
};

int Sim::n_iterations()
{
   return n_iters;
};

int Sim::iteration() 
{
   return iter;
};

int Sim::process()
{
   return proc;
};

#ifdef SIM_MPI
void Sim::signal(const SimSignal& sig, const std::string& str)
{
   SIM_DEBUG("proc="<<proc << " iter="<<iter, "signal="<<sig<<" msg="<<str)

   sim_send_signal(sig, iter, SIM_MPI_CNTRL);

   if (is_io_signal(sig))
   {
      sim_send_message(str, SIM_MPI_CNTRL);
   };

   if (is_throw_signal(sig))
   {
      throw sig;
   };
};

void Sim::output_level(const SimSignal& sig)
{
   int s = to_output_signal(sig);
   sim_send_signal(s, iter, SIM_MPI_CNTRL);
};

#else  //no MPI
void Sim::signal(const SimSignal& sig, const std::string& str)
{
   SIM_DEBUG("no MPI! proc="<<proc << " iter="<<iter, "signal="<<sig<<" msg="<<str)
   // todo

   if (is_io_signal(sig))
   {
      iomngr->message(str, sig, iter);
   };

SIM_DEBUG("is_throw_signal", is_throw_signal(sig) )

   if (is_throw_signal(sig))
   {

SIM_DEBUG("throwing sig=", sig)

      throw sig;
   };
};

void Sim::output_level(const SimSignal& sig)
{
   iomngr->set_output_level(to_output_signal(sig));
};
#endif



bool Sim::defined(const ExprNameT& name)
{
   return (scope.defined(name));
};

bool Sim::defined_as(const ExprNameT& name, const ExprNameT& name2)
{
   ExprPtrT e = scope.match(name);
   if (e == ExprNoMatchPtr()) return false;
   return (e->name() == name2);
};

/*
bool Sim::defined_as_creator(const ExprNameT& name)
{
   return defined_as(name, ExprCreator::Name);
};
*/

std::string Sim::info()
{
   SIM_DEBUG("Sim.info()", "")

   std::stringstream str;
   str << "Sim: process = " << process() << " iteration = " << iteration() << 
   str << " n_iterations = " << n_iterations() << std::endl;
   str << "global = " << global << std::endl;
   str << "scope:" << std::endl << scope.info();
   return str.str();
}; 



bool Sim::get_expr(const ExprNameT& n, ExprPtrT& e, const SimSignal& sig)
{
   bool eo;
   if (!get_expr_nothrow(n,e, sig, eo))
   {
      if (eo) err << sig;
      return false;
   };
   return true;
};

bool Sim::get(const ExprNameT& n, ExprPtrT& e, const SimSignal& sig)
{
   return get_expr(n,e,sig);
};


bool Sim::get_definition(const ExprNameT& n, ExprPtrT& e, const SimSignal& sig)
{
   bool eo = is_io_signal(sig);
   e = scope.match(n);
   if (e == ExprNoMatchPtr())
   {
      if (eo) err << "Cannot get definition for " << n << "!" << sig;
      e = ExprNullPtr();
      return false; 
   }
   return true;
};

bool Sim::define(const ExprNameT& name, const ExprPtrT& expr)
{
   scope.define(name, expr);
   return true;
};


// private member functions


// inline
bool Sim::get_expr_nothrow(const ExprNameT& n, ExprPtrT& e, const SimSignal& sig, bool& eo)
{
   eo = is_io_signal(sig);

   try 
   {
      e = scope.match(n);
      if (e == ExprNoMatchPtr())
      {
         if (eo) err << "Cannot get value for " << n << "! Not Defined!";
         e = ExprNullPtr();
         return false; 
      }
      e = e->evaluate(&scope);
   }
   catch (const ExpressionError& exprerr)
   {
      if (eo) err << exprerr.what();
      e = ExprNullPtr();
      return false;
   };
   return true;
};


std::string  Sim::file_extension()
{
   std::ostringstream str;
   str.width(4);
   str.fill('0');
   str << iteration();
   return std::string(".") + str.str();
};




