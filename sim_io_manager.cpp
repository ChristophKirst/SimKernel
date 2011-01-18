/**************************************************************************
   sim_io_manager.cpp - input output management

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
***************************************************************************/
#include "sim_io_manager.h"

std::map<SimSignal, std::string> SimIOManager::SimSignalDescription;

SimIOManager::SimIOManager() {};
SimIOManager::~SimIOManager() {};

void SimIOManager::init(bool so,  const std::string& prefix, bool pid,
             bool fo, const std::string& filen, 
             const SimSignal& ol)
{
   messages.clear();
   errors.clear();
   messages.push_back(""); // global messages with id = -1
   errors.push_back("");   // global errors with id = -1
   n_ids = messages.size();

   next_pre = true;

   pre = prefix;
   print_id = pid;

   outl = ol;

   // screen output
   screen_out = so;

   // log file 
   file_out = fo; 
   if (file_out) 
   {
      file_name = filen;
      of.open (file_name.c_str() , std::ios::out | std::ios::trunc);
      if (!of.is_open() || !of.good()) 
      {
         std::ostringstream str;
         str << pre << std::string("Cannot open message file: ") + file_name << " !" << std::endl;
         errors[0] += str.str();
         std::cout << str.str();
         file_out = false;
      };
   };


   SimSignalDescription.insert(std::pair<SimSignal, std::string>(Message, ""));
   SimSignalDescription.insert(std::pair<SimSignal, std::string>(Warning, "Warning: "));
   SimSignalDescription.insert(std::pair<SimSignal, std::string>(Abort,   "Error: "     )); 
   SimSignalDescription.insert(std::pair<SimSignal, std::string>(Exit,    "Exit: "));

};

void SimIOManager::set_iterations(int n_iters)
{
   for (int i = 0; i <= n_iters; i++)
   {
      messages.push_back("");
      errors.push_back("");
   };
   n_ids = messages.size();
};

void SimIOManager::set_output_level(const SimSignal& sig)
{
   outl = sig;
};


void SimIOManager::message(const std::string& tex, const SimSignal& sig, int id, int pr)
{
   if (sig >= outl)
   {
      std::string msg = message_text(tex, sig);
      if (is_error_signal(sig)) errors[id+1] += msg;
      else messages[id+1] += msg;

      if (next_pre == false && is_error_signal(sig)) endline(id, pr);
      print_message(msg, id, pr);
   };
};

void SimIOManager::print_message(const std::string& tex, int id, int pr)
{
   //  output
   std::ostringstream str;
   std::string::const_iterator i = tex.begin();
   if (i != tex.end() && next_pre) str << printpre(id, pr);
   next_pre = false;
   while(i != tex.end())
   {
      str << *i;
      if (*i == '\n')
      {
         if (i != tex.end()-1)
         {
            str << printpre(id, pr);
            next_pre = false;
         } else {
            next_pre = true;  //new line but no prefix printed at end 
         }
      };
      i++;
   };

   if (file_out) of << str.str();
   if (screen_out) std::cout << str.str();
   flush();
};

std::string SimIOManager::message_text(const std::string& tex, const SimSignal& sig)
{
   std::ostringstream str;
   str << SimSignalDescription[sig];
   std::string::const_iterator i = tex.begin();
   while(i != tex.end())
   {
      str << *i;

      if (*i == '\n')
      {
         if (i != tex.end()-1)
         {
            str << SimSignalDescription[sig];
         } else {
            return str.str();
         };
      };
      i++;
   };
   str << "\n";
   return str.str();
};

std::string SimIOManager::printpre(int id, int pr)
{
   std::ostringstream str;
   str << pre;
   if (print_id && pr != -1) str << "#" << pr << ": "; 
   if (print_id && id != -1) str << "It " << id << ": ";
   return str.str();
};

void SimIOManager::set_screen_output(bool so)
{
   screen_out = so;
};

void SimIOManager::set_pre(const std::string& newpre)
{
   pre = newpre;
};

std::string SimIOManager::message_summary_text(int id)
{
   std::ostringstream str;
   if (id == -1)
   {
      str << "\n\nMessage Summary:\n";
      if (messages[0] != "")
      {
         str << "Messages: General:\n";
         str << messages[0];
         str << "\n";
      };
      for (int i = 1; i < n_ids; i++)
      {
         if (messages[i] != "")
         {
            str << "Messsages: It " << (i-1) << ":\n";
            str << messages[i];
            str << '\n';
         };
      };
   }
   else
   {
      str << "Messsages: It " << id << ":";
      if (messages[id+1] != "")
      {
         str << '\n' << messages[id+1] << '\n';
      } else {
         str << " No messages!\n";
      };
   };
   return str.str();
};

std::string SimIOManager::error_summary_text(int id)
{
   std::ostringstream str;
   if (id == -1)
   {
      str << "\nError Summary:\n";
      bool e = false;
      if (errors[0] != "")
      {
         e = true;
         str << "Errors: General:\n";
         str << errors[0];
         str << "\n";
      }
      for (int i = 1; i < n_ids; i++)
      {
         if (errors[i] != "")
         {
            e = true;
            str << "Errors: It " << (i-1) << ":\n";
            str << errors[i];
            str << '\n';
         };
      };
      if (!e) str << "No errors!\n";
   }
   else
   {
      str << "Errors: It " << id << ":";
      if (errors[id+1] != "")
      {
         str << '\n' << errors[id+1] << '\n';
      } else {
         str << " No errors!\n";
      };
   };
   return str.str();
};

void SimIOManager::message_summary(int id)
{
   print_message(message_summary_text(id), id);
};

void SimIOManager::error_summary(int id)
{
   print_message(error_summary_text(id), id);
};

void SimIOManager::summary(int id)
{
   message_summary(id);
   error_summary(id);
};

void SimIOManager::flush()
{
   if (screen_out) std::cout.flush();
   if (file_out) of.flush();
};

void SimIOManager::endline(int id, int pr)
{
   // save message for summary
   assert(id+1 < n_ids);
   if (id > -2) messages[id+1] += "\n"; 

   // output
   std::string str("");
   if (next_pre) str = printpre(id, pr);

   if (screen_out) std::cout << str << std::endl;
   if (file_out) of << str << std::endl;

   next_pre = true;
};

void SimIOManager::close()
{
   flush();
   //endline();
   if (file_out) of.close();
};

