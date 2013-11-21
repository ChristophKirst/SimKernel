/***************************************************************************
   sim_signals.cpp   -   signals for messaging and error system

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#include "sim_signals.h"

bool is_io_signal(const int& sig)
{
   return (sig == int(Message) || sig == int(Warning) 
        || sig == int(Abort)   || sig == int(Exit));
};

bool is_error_signal(const int& sig)
{
   return (sig == Warning || sig == Abort || sig == Exit);
};

bool is_throw_signal(const int& sig)
{
   return (sig == Abort || sig == Exit);
};

bool is_io_output_signal(const int& sig)
{
   return (  sig == AllOutput || sig == NoMessage || sig == NoWarning
          || sig == NoAbort   || sig == NoExit    || sig == NoOutput);
};

SimSignal to_output_signal(const SimSignal& sig)
{
   if (sig == Success) return AllOutput;
   int s = sig;
   while (!is_io_output_signal(s)) s--;
   return SimSignal(s);
};
