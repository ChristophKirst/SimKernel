/***************************************************************************
   sim.hpp -   sim template implementations

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/

template<typename T>
bool Sim::get(const ExprNameT& n, T& dest, const SimSignal& sig)
{
   ExprPtrT e; bool eo;
   if (!get_expr_nothrow(n, e, sig, eo))
   {
      if (eo) err << " Parameter " << n << " not set! Aborting." <<sig;
      // return false;
      exit(1);
   };

   try
   {
      ExprToValue(e,dest);
   }
   catch (const ExpressionError& exprerr)
   {
      if (eo) err << exprerr.what() << " Parameter " << n << " not set! Aborting." << sig;
      // return false;
      exit(1);
   };

   return true;
};

template<typename T, typename D>
bool Sim::get(const ExprNameT& n, T& dest, const D& def, const SimSignal& sig)
{
   ExprPtrT e; bool eo;
   if (!get_expr_nothrow(n, e, sig, eo))
   {
      if (eo) err << " Using " << toExprString(def) << " instead!" << sig;
      dest = def;
      return false;
   };

   try
   {
      ExprToValue(e,dest);
   }
   catch (const ExpressionError& exprerr)
   {
      if (eo) err << exprerr.what() << " Using " << toExprString(def) << " instead!" << sig;
      dest = T(def);
      return false;
   };

   return true;
};


template<typename T, typename D>
bool Sim::get(const ExprNameT& name, std::vector<T>& l, const D& def, const SimSignal& sig)
{
   return get_list(name, l, def, sig);
};

template<typename T, typename D>
bool Sim::get(const ExprNameT& name, std::vector< std::vector<T> >& l, const D& def, 
                                                                              const SimSignal& sig)
{
   return get_matrix(name, l, def, sig);
};



template<typename T, typename D>
bool Sim::expr_to_vector_nothrow(const ExprPtrT& e, std::vector<T>& l, const D& def, bool eo)
{
   if (!e->listQ()) 
   {
      if (eo) err << e << " is not a List!";
      return false;
   };

   bool succ = true;
   l.clear();
   for (ExprArgIteratorT it = e->arg.begin(); it != e->arg.end(); it++)
   {
      try
      {
         T val; ExprToValue(*it, val);
         l.push_back(val);
      }
      catch (const ExpressionError& exprerr)
      {
         if (eo) err << exprerr.what() << "\n";
         l.push_back(T(def));
         succ = false;
      };
   };
   if (!succ && eo) err << "Using " << toExprString(def) << " instead!";
   return succ;
};


template<typename T, typename D>
bool Sim::get_list(const ExprNameT& name, std::vector<T>& l, const D& def, const SimSignal& sig)
{
   ExprPtrT e; bool eo;
   if (!get_expr_nothrow(name, e, sig, eo))
   {
      if (eo) err << " Cannot convert " << name << " to List!" << sig;
      return false;
   };
   if (!expr_to_vector_nothrow<T,D>(e, l, def, eo))
   {
      if (eo) err << " Cannot convert " << name << " to List!" << sig;
      return false;
   };
   return true;
};




template<typename T, typename D>
bool Sim::get_matrix(const ExprNameT& name, std::vector< std::vector<T> >& l, const D& def, 
                                                                              const SimSignal& sig)
{
   ExprPtrT e; bool eo;
   if (!get_expr_nothrow(name, e, sig, eo))
   {
      if (eo) err << " Cannot convert " << name << " to a matrix!" << sig;
      return false;
   };

   if (!e->listQ()) 
   {
      if (eo) err << name << " = " << e << " is not a matrix!" << sig;
      return false;
   };

   l.clear();
   bool succ = true;
   for (ExprArgIteratorT it = e->arg.begin(); it != e->arg.end(); it++)
   {
      l.push_back(std::vector<T>());
      if (!expr_to_vector_nothrow((*it), (*(l.end()-1)), def, eo))
         succ = false;
   };

   if (!succ && eo) err << " Error in converting " << name << " = " << e << " to a matrix!" << sig;
   return succ;
};



template<typename T>
bool Sim::define(const ExprNameT& name, const T& value, const SimSignal& sig)
{
   ExprPtrT e;
   try 
   {
      e = ValueToExpr(value);
   } 
   catch (const ExpressionError& exprerr) 
   {
      if (is_io_signal(sig)) err << exprerr.what() << sig;
      return false;
   };
   scope.define(name, e);
   return true;
};


template <typename T>
Sim::expr_t Sim::to_expr(const T& value, const SimSignal& sig)
{
   ExprPtrT e;
   try 
   {
      e = ValueToExpr(value);
   } 
   catch (const ExpressionError& exprerr) 
   {
      if (is_io_signal(sig)) err << exprerr.what() << sig;
      return ExprNullPtr();
   };
   return e;
};


template <typename T>
bool Sim::to_value(const ExprPtrT& e, T& dest, const SimSignal& sig)
{
   try
   {
      ExprToValue(e,dest);
   }
   catch (const ExpressionError& exprerr)
   {
      if (is_io_signal(sig)) err << exprerr.what() << sig;
      return false;
   };
   return true;
};


template<typename T, typename D>
bool Sim::to_list(const ExprPtrT& e, std::vector<T>& l, const D& def, const SimSignal& sig)
{
   bool eo = is_io_signal(sig);
   if (!expr_to_vector_nothrow<T,D>(e, l, def, eo))
   {
      if (eo) err << " Cannot convert " << e << " to List!" << sig;
      return false;
   };
   return true;
}


template<typename T, typename D>
bool Sim::to_matrix(const ExprPtrT& e, std::vector< std::vector<T> >& l, const D& def, const SimSignal& sig)
{
   bool eo = is_io_signal(sig);
   if (!e->listQ()) 
   {
      if (eo) err << e << " is not a matrix!" << sig;
      return false;
   };

   l.clear();
   bool succ = true;
   for (ExprArgIteratorT it = e->arg.begin(); it != e->arg.end(); it++)
   {
      l.push_back(std::vector<T>());
      if (!expr_to_vector_nothrow((*it), (*(l.end()-1)), def, eo))
         succ = false;
   };

   if (!succ && eo) err << " Error in converting " << e << " to a matrix!" << sig;
   return succ;
};
