/***************************************************************************
   sim_io_manager.h  -  manages messages and errors

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#ifndef SIM_IO_MANAGER_H
#define SIM_IO_MANAGER_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <assert.h>

#include "sim_signals.h"

class SimIOManager
{
private:
   bool screen_out;

   bool file_out;
   std::string file_name;
   std::ofstream of;

   std::vector<std::string>  messages;
   std::vector<std::string>  errors;
   int n_ids;

   std::string pre;
   bool print_id;

   bool next_pre;

   SimSignal outl;

   static std::map<SimSignal, std::string> SimSignalDescription;

public:

   SimIOManager();
   ~SimIOManager();

   void init(bool so = true,  const std::string& prefix = "Sim: ", bool pid = true,
             bool fo = false, const std::string& filen  = "sim.log", 
             const SimSignal& ol = AllOutput);

   void set_iterations(int n_iters);

   void set_output_level(const SimSignal& sig);

   void message(const std::string& tex, const SimSignal& sig=Message, int id = -1, int pr =-1);

   void set_screen_output(bool so);

   void set_pre(const std::string& newpre);

   void message_summary(int id = -1);
   void error_summary(int id = -1);
   void summary(int id = -1);

   void flush();

   void endline(int id = -1, int pr = -1);

   void close();

private:
   void print_message(const std::string& tex, int id = -1, int pr = -1);

   std::string message_text(const std::string& tex, const SimSignal& sig);

   std::string printpre(int id, int pr);

   std::string message_summary_text(int id = -1);

   std::string error_summary_text(int id = -1);
};


#endif
