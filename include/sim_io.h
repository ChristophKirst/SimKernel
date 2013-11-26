/***************************************************************************
   sim_io.h  -  Sim object's message and error objects

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#ifndef SIM_IO_H
#define SIM_IO_H

#include <string>
#include <sstream>

#include "sim_signals.h"

//#include <iostream>
//#define SIO_DEBUG(s1,s2) std::cout << "SimIO DEBUG: " << s1 << " " << s2 << std::endl; std::cout.flush();
#define SIO_DEBUG(s1,s2) 

class Sim;

class SimIO
{
private:
   Sim* sim;
   std::string msg;

public:

   SimIO(Sim* sim_);
   ~SimIO();

   template <typename T>
   SimIO& operator << (T tex)
   {
      SIO_DEBUG("Operator <<", tex)

      std::ostringstream str;
      str << tex;
      msg += str.str();
      return (*this);
   }

   SimIO& operator << (const SimSignal& sig);

   // endl.
   SimIO& operator << (SimIO& (*io) (SimIO&));

   void endline();
   void flush(const SimSignal& sig = Message);
   void release(const SimSignal& sig = Warning);

   void output_level(const SimSignal& sig);
};

// endl
SimIO& Endl (SimIO& m);


#endif
