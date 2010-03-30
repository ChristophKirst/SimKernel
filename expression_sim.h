/***************************************************************************
   expression_sim.h   -  sim specific expressions

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#ifndef EXPRESSION_SIM_H
#define EXPRESSION_SIM_H

#include <string>
#include <sstream>
#include <assert.h>
#include <iostream>
#include <vector>
#include <deque>

#include "expression.h"
#include "expression_basic.h"


//SimControl: reset_iteration() = init_iteration()
//            next_iteration()
//            bool end_iteration()
//            unsigned int n_interations()

//#include <iostream>
//#define EXPR_SIM_DEBUG(s1, s2) std::cout << "ExprDebug: SIM: " << s1 << " "  << s2 << std::endl; std::cout.flush();
#define EXPR_SIM_DEBUG(s1, s2) 


//Creator[expr]  evaluates expr after iterators have been defined
//               checks for expr->listQ()
class ExprCreator : public Expression
{
   public:
      ExprPtrT list;
      ExprIndexT list_step;
      ExprPtrT step, start, end, delta;

      typedef std::vector<ExprPtrT> CreatorListT;
      static CreatorListT creators;


      enum CreatorStatusT {
                           Ndef = 0,  // kernel initialization
                           Init,      // Creator initialization
                           Done       // ready
                          };

      CreatorStatusT status;

   public:
      EXPR_NAME_DECL()

      ExprCreator() 
      : Expression(), status(Ndef),
        list(ExprNullPtr()), list_step(0),
        step(ExprNullPtr()), end(ExprNullPtr()), delta(ExprNullPtr())
      {
         creators.push_back(ExprPtrT(this));
      };

      // Creator[{expr1, expr2, ...}] (list creator)
      ExprCreator(const ExprPtrT& l) 
      : Expression(l), status(Ndef),
        list(ExprNullPtr()), list_step(0),
        step(ExprNullPtr()), end(ExprNullPtr()), delta(ExprNullPtr())
      {
         creators.push_back(ExprPtrT(this));
      };
 
      // Creator[expr, {i,s,e,d}]  (works as Table)
      ExprCreator(const ExprPtrT& b, const ExprPtrT& it) 
      : Expression(b, it),  status(Ndef),
        list(ExprNullPtr()), list_step(0),
        step(ExprNullPtr()), end(ExprNullPtr()), delta(ExprNullPtr())
      {
         creators.push_back(ExprPtrT(this));
      };

      static void init(ExprScopeT* scope)
      {
         EXPR_SIM_DEBUG("Creator: init()", "")
         for (CreatorListT::iterator i  = creators.begin(); i != creators.end(); i++)
         {
            ((ExprCreator*)(*i).get())->status = Init;
         };

         for (CreatorListT::iterator i  = creators.begin(); i != creators.end(); i++)
         {
            EXPR_SIM_DEBUG("Creator: init() creators:", (*i))
            (*i)->evaluate(scope);
         };

         for (CreatorListT::iterator i  = creators.begin(); i != creators.end(); i++)
         {
            //EXPR_EVAL_ASSERT(((ExprCreator*)(*i).get())->status == Defd, CreatorError, this)
            ((ExprCreator*)(*i).get())->status = Done;
         };
      };

   public:
      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_SIM_DEBUG("Creator: evaluate():", print())
         EXPR_SIM_DEBUG("Creator: evaluate(): status=", status)

         // the iteration value 
         if (status == Done)
         {
            if (nargs() == 1) // list creator
            {
               ExprPtrT value = list->arg[list_step]->evaluate(scope);
               list_step++;
               if (list_step == list->nargs()) list_step = 0;
               return value;
            } else { // start, end, delta creator
               scope->push();
               scope->define_local(arg[1]->arg[0]->symbolname(), step);
                  ExprPtrT ret = arg[0]->evaluate(scope);
               scope->pop();
               if (step->integerQ() && delta->integerQ())
               {
                  int s = int(*step) + int(*delta);
                  if (s > int(*end)) step = start; 
                  else step = ExprPtrT(new ExprInteger(s));
               } else {
                  double s = double(*step) + double(*delta);
                  if (s > double(*end)) step = start; 
                  else step = ExprPtrT(new ExprReal(s));
               };
               return ret;
            };
         };

         // Kenerl initialization
         if (status == Ndef) return ExprPtrT(this);

         // Creator initialization
         if (status == Init)
         {
            if (nargs() == 1) // list iterator
            {
               list = arg[0]->evaluate(scope);
               list_step = 0;
               EXPR_EVAL_ASSERT(list->listQ() && list->nargs()>0, CreatorExpectList, this);
            } else { // nargs() == 2  // start, end, delta iterator
               start = arg[1]->arg[1]->evaluate(scope);
               end = arg[1]->arg[2]->evaluate(scope);
               delta = ExprPtrT(new ExprInteger(1));
               if (arg[1]->nargs()>3) delta = arg[1]->arg[3]->evaluate(scope);
               step = start;
               EXPR_EVAL_ASSERT(start->numberQ() && end->numberQ() && delta->numberQ(), CreatorExpectNumber, this)
            };
            return ExprNullPtr();
         };
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() == 1) return NoSyntaxError;
         if (nargs() == 2)
         {
            if (!(arg[1]->listQ() && (arg[1]->nargs() == 4 || arg[1]->nargs() ==3))) return CreatorExpectIterationList;
            if (!(arg[1]->arg[0]->symbolQ())) return CreatorExpectSymbol;
            return NoSyntaxError;
         };
         return CreatorSyntaxError;
      };
};



class ExprIterator : public Expression
{
   public:
      typedef std::vector<ExprPtrT> IteratorListT;
      static IteratorListT iterators;

      typedef std::deque<unsigned int> IteratorOrderT;
      static IteratorOrderT ordering;  // dependency ordering - most dependend is last

      static int count;      // actual iteration number
      static int level;      // actual level for increasing the iterator

      ExprIndexT id; // position in IteratorList

      ExprPtrT step, end, delta;  // iteration varaible
      ExprPtrT list;              // list iterator
      ExprPtrT value;             // value of iterated variable or list
      ExprIndexT list_step;       // actual position in list

      enum IteratorStatusT {Ndef = 0,  // initial initialization of kernel
                                        // retrun unevaluated pointer to this Iterator 
                                        // -> all non-iterated parameter are now defined
                                        // -> iterator symbols are defined as ExprIterator
                             Init,      // init() sets status to Init and evaluates all
                                        // iterators then status is set to First
                                        // -> automatic dependency ordering (assert no loops !)
                                        // -> value, end, delta or list are set (assert this prop)
                             Defd,      // init is done (exclude reevaluation by second symbol ref)
                                        // -> returns unevaluated pointer to this Iterator
                             Eval,      // Iterator is evaluated -> to exclude loop dependencies !
                             Done,      // return actual iteration values !
                                        // 
                                        // usage: 
                                        // rootkernel->evaluate(scope)
                                        // ExprIterator::init(scope); ??
                                        // while (ExprIterator::next_iteration(scope)) { ... };
                             };

      IteratorStatusT status;

   public:
      EXPR_NAME_DECL()

      ExprIterator() : Expression(), status(Ndef),
        value(ExprNullPtr()), list(ExprNullPtr()), list_step(0),
        step(ExprNullPtr()), end(ExprNullPtr()), delta(ExprNullPtr())
      {
         id = iterators.size();
         iterators.push_back(ExprPtrT(this));
         count = -1;
      };

      // Iterator[{it1, it2, ...}] (iterate over list)
      ExprIterator(const ExprPtrT& l) : Expression(l), status(Ndef),
        value(ExprNullPtr()), list(ExprNullPtr()), list_step(0),
        step(ExprNullPtr()), end(ExprNullPtr()), delta(ExprNullPtr())
      {
         id = iterators.size();
         iterators.push_back(ExprPtrT(this));
         count = -1;
      };

      // Iterator[expr, {i,s,e,d}]  (works as Table)
      ExprIterator(const ExprPtrT& b, const ExprPtrT& it) 
      : Expression(b, it),  status(Ndef),
        value(ExprNullPtr()), list(ExprNullPtr()), list_step(0),
        step(ExprNullPtr()), end(ExprNullPtr()), delta(ExprNullPtr())
      {
         id = iterators.size();
         iterators.push_back(ExprPtrT(this));
         count = -1;
      };

      static int count_iterations(ExprScopeT* scope)
      {
         // assume kernel is initialized
         count = -1;
         int n = 0;
         while(next_iteration(scope)) {n++;};
         count = -1;
         return n;
      };

      static int n_iteration() { return count; };

      static void init(ExprScopeT* scope)
      {
         EXPR_SIM_DEBUG("Iterator: init()", "")
         ordering.clear();

         for (IteratorListT::iterator i  = iterators.begin(); i != iterators.end(); i++)
         {
            ((ExprIterator*)(*i).get())->status = Init;
         };

         for (IteratorListT::iterator i  = iterators.begin(); i != iterators.end(); i++)
         {
            EXPR_SIM_DEBUG("Iterator: init() iterators: ", (*i))
            (*i)->evaluate(scope);
         };

         for (IteratorListT::iterator i  = iterators.begin(); i != iterators.end(); i++)
         {
            EXPR_EVAL_ASSERT(((ExprIterator*)(*i).get())->status == Defd, IteratorInternalError, (*i))
            ((ExprIterator*)(*i).get())->status = Done;
         };
      };

      static bool next_iteration(ExprScopeT* scope)
      {
         EXPR_SIM_DEBUG("Iterator: next_iteration", count)
         count++;
         level = 0;
         //no iterators -> iterate once 
         if (iterators.size() == 0)
         {
            if (count == 0)
            {
               ExprCreator::init(scope);
               return true;
            } else return false;
         }; 

         // first call -> initialize iterators
         if (count == 0) {init(scope); ExprCreator::init(scope); return true;};

         // increase most dependent iterator
         if ( ((ExprIterator*)(iterators[ordering[0]].get()))->increase_iteration(scope) )
         {
            ExprCreator::init(scope); return true;
         } else return false;
      };

      bool increase_iteration(ExprScopeT* scope)
      {
         EXPR_SIM_DEBUG("Iterator: increase_iteration:", print())

         if (nargs() == 1) //list
         {
            list_step++;
            if (list_step == list->nargs())
            {
               level++;
               if (level == iterators.size()) return false;
               if (!((ExprIterator*)(iterators[ordering[level]].get()))->increase_iteration(scope))
                  return false;
               list_step = 0;
               status = Eval;
               list = arg[0]->evaluate(scope);
               status = Done;
               EXPR_EVAL_ASSERT(list->listQ() && list->nargs()>0, IteratorExpectList, this);
            };
            value = list->arg[list_step];
            return true;

         } else { // nargs() == 2:  start, end, delta iterator

            bool next = false;

            if (step->integerQ() && delta->integerQ())
            {
               int s = int(*step) + int(*delta);
               step = ExprPtrT(new ExprInteger(s));
               next = (s > int(*end));
            } else {
               double s = double(*step) + double(*delta);
               step = ExprPtrT(new ExprReal(s));
               next = (s > double(*end));
            };

            if (next)
            {
               level++;
               if (level > iterators.size()) return false;
               if (!((ExprIterator*)(iterators[ordering[level]].get()))->increase_iteration(scope))
                  return false;

               // initialize the iterator anew
               status = Eval;
               step = arg[1]->arg[1]->evaluate(scope);
               end = arg[1]->arg[2]->evaluate(scope);
               delta = arg[1]->arg[3]->evaluate(scope);
               EXPR_EVAL_ASSERT(step->numberQ() && end->numberQ() && delta->numberQ(), IteratorExpectNumber, this)
               //status = Done;
            };

            status = Eval;
            scope->push();
            scope->define_local(arg[1]->arg[0]->symbolname(), step);
            value = arg[0]->evaluate(scope);
            scope->pop();
            status = Done;
            return true;
         };
      };

   public:
      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_SIM_DEBUG("Iterator: evaluate: status=", int(status))

         EXPR_EVAL_ASSERT(status != Eval, IteratorLoop, this)

         // the iteration value 
         if (status == Done) return value;

         // Kernel initialization
         if (status == Ndef) return ExprPtrT(this);

         // Iterator initialization
         if (status == Init)
         {
            if (nargs() == 1) // list iterator
            {
               status = Eval;
               list = arg[0]->evaluate(scope);
               list_step = 0;
               EXPR_EVAL_ASSERT(list->listQ() && list->nargs()>0, IteratorExpectList, this);
               value = list->arg[0]->evaluate(scope);
               status = Defd;
            } else { // nargs() == 2  // start, end, delta iterator
               status = Eval;
               step = arg[1]->arg[1]->evaluate(scope);
               end = arg[1]->arg[2]->evaluate(scope);
               delta = ExprPtrT(new ExprInteger(1));
               if (arg[1]->nargs()>3) delta = arg[1]->arg[3]->evaluate(scope);
               EXPR_EVAL_ASSERT(step->numberQ() && end->numberQ() && delta->numberQ(), IteratorExpectNumber, this)
               scope->push();
               scope->define_local(arg[1]->arg[0]->symbolname(), step);
               value = arg[0]->evaluate(scope);
               scope->pop();
               status = Defd;
            };

            // automatic dependency ordering
            ordering.push_front(id);
        };

         // Iteration initialization evaluation call after initializing
         if (status = Defd) return value;

         EXPR_EVAL_ASSERT(false, IteratorInternalError, this)
         return ExprNullPtr();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() == 1) return NoSyntaxError;
         if (nargs() == 2)
         {
            if (!(arg[1]->listQ() && (arg[1]->nargs() == 4 || arg[1]->nargs()==3))) return IteratorExpectIterationList;
            if (!(arg[1]->arg[0]->symbolQ())) return IteratorExpectSymbol;
            return NoSyntaxError;
         };
         return IteratorSyntaxError;
      };

};





#endif

