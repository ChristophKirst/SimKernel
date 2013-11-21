/***************************************************************************
   sim_io.cpp  -  Sim object's message and error objects

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#include "sim_io.h"
#include "sim_signals.h"
#include "sim.h"

SimIO::SimIO(Sim* sim_) : sim(sim_), msg("") {};

SimIO::~SimIO() 
{
   //release(Warning);

};

void SimIO::release(const SimSignal& sig)
{
   if (msg!= "") 
   {
      std::ostringstream str;
      str << "There was an unsent message: " <<  msg;
      sim->signal(sig, str.str());
      msg = "";
   };
};

SimIO& SimIO::operator << (const SimSignal& sig)
{
   std::string m = msg;
   msg = "";
   sim->signal(sig, m);
   return (*this);
};

// endl.
SimIO& SimIO::operator << (SimIO& (*io) (SimIO&))
{
   return (*io)(*this);
};

void SimIO::endline()
{
   msg += "\n";
   flush();
};

void SimIO::flush(const SimSignal& sig)
{
   if (msg != "") sim->signal(sig, msg);
   msg = "";
};

SimIO& Endl (SimIO& io)
{
   io.endline();
   return (io);
};

void SimIO::output_level(const SimSignal& sig)
{
   sim->output_level(sig);
};

