/***************************************************************************
   sim.h   -   interface between parameter expressions 
               and simulation kernel

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#ifndef SIM_H
#define SIM_H

#include "expression.h"
#include "expression_basic.h"
#include "expression_sim.h"

#include "sim_io.h"
#include "sim_signals.h"

#ifndef SIM_MPI
#include "sim_io_manager.h"
#endif

//#define SIM_DEBUG(s1, s2) std::cout<< "Sim Debug: " << s1 << " " << s2 << std::endl; std::cout.flush();
#define SIM_DEBUG(s1, s2)

class Sim
{
   public:
      typedef ExprPtrT expr_t;

   public:
      // io object
      SimIO          io;

      // io for internal errors
      SimIO          err;

      // io for internal errors
      SimIO          msg;

   public:

      //expression kernel
      ExprPtrT       global;
      ExprScopeT     scope;


   private:
#ifndef SIM_MPI
      SimIOManager*  iomngr;
#endif


   private:
      //total number of iterations and actual iteration
      int            n_iters;
      int            iter;

      //process number
      int            proc;

   public:
      Sim();
      Sim(const ExprPtrT& g);
      ~Sim() {};

   public:
      void define_global(const ExprPtrT& g);
      void clear();
      bool init(const ExprPtrT& g, std::string& err, int p = -1);
#ifndef SIM_MPI
      void init_io(SimIOManager*);
#endif

      bool next_iteration();
      int  n_iterations();
      int  iteration();
      
      int  process();

      // signalling
      void output_level(const SimSignal& sig);

      void signal(const SimSignal& sig, const std::string& str = "");

      std::string info();

      std::string file_extension();
      
      std::string iteration_info();
      
      bool init_result_file;
      bool append_data_to_result_file(std::string result_file, std::string data_file);
      

   public:
      bool defined(const ExprNameT& name);
      bool defined_as(const ExprNameT& name, const ExprNameT& name2);

      bool get_definition(const ExprNameT& n, ExprPtrT& e, const SimSignal& = Warning);

      bool get(const ExprNameT& n, ExprPtrT& e, const SimSignal& = Warning);

      bool get_expr(const ExprNameT& n, ExprPtrT& e, const SimSignal& = Warning);

      template<typename T>
      bool get(const ExprNameT& n, T& dest, const SimSignal& = Warning);

      template<typename T, typename D>
      bool get(const ExprNameT& n, T& dest, const D& def, const SimSignal& = Warning);

      template<typename T, typename D>
      bool get(const ExprNameT& name, std::vector<T>& l, const D& def, 
                                                          const SimSignal& = Warning);

      template<typename T, typename D>
      bool get(const ExprNameT& name, std::vector< std::vector<T> >& l, const D& def,
                                                          const SimSignal& = Warning);

      template<typename T, typename D>
      bool get_list(const ExprNameT& name, std::vector<T>& l, const D& def, 
                                                          const SimSignal& = Warning);

      template<typename T, typename D>
      bool get_matrix(const ExprNameT& name, std::vector< std::vector<T> >& l, const D& def,
                                                          const SimSignal& = Warning);

      bool define(const ExprNameT& name, const ExprPtrT& expr);

      template<typename T>
      bool define(const ExprNameT& name, const T& value, const SimSignal& = Warning);

      template<typename T>
      bool to_value(const ExprPtrT& e, T& value, const SimSignal& = Warning);

      template<typename T, typename D>
      bool to_list(const ExprPtrT& e, std::vector<T>& value, const D& def, 
                                                             const SimSignal& = Warning);

      template<typename T, typename D>
      bool to_matrix(const ExprPtrT& e, std::vector< std::vector<T> >& value, const D& def,
                                                             const SimSignal& = Warning);

      template <typename T>
      expr_t to_expr(const T& val, const SimSignal& = Warning);


  private:

      // Expression related 
      bool get_expr_nothrow(const ExprNameT& n, ExprPtrT& e, const SimSignal& sig, bool& eo);

      template<typename T, typename D>
      bool expr_to_vector_nothrow(const ExprPtrT& e, std::vector<T>& l, const D& def, bool eo);
};

// template implementations
#include "sim.hpp"


#endif
