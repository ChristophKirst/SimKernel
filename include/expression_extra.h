/***************************************************************************
   expression_extra.h   -  usefull extra expressions 
   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#ifndef EXPRESSION_EXTRA_H
#define EXPRESSION_EXTRA_H

#include <typeinfo>
#include <string>
#include <fstream>

#include <time.h>
#include <sys/time.h>

//Mac Os does not have clock_gettime
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

//#include <gsl/gsl_rng.h>

#include "expression.h"
#include "expression_basic.h"


/************************************************************************

   File IO

************************************************************************/
//Import["filename"]
//imports text data to list using >> operator to convert values 
//non numbers are parsed as string expressions



class ExprImport : public Expression
{
   public: 
      ExprImport() : Expression() {};

      ExprImport(const ExprPtrT& fn) : Expression(fn)
      {};

   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT fn = arg[0]->evaluate(scope);
         ExprPtrT l(new ExprList());

         if (!(fn->stringQ())) return l;

         // open file
         std::ifstream file;
         file.open(std::string(*fn).c_str());
         if (!file.good()) return l;

         ExprPtrT ll(new ExprList());
         std::string s;
         char c = ' ';
         bool nl;

         while((c == '\t' || c == '\n' || c==' ' || c == '\r') && !file.eof() && file.good())
         {
            c = file.get();
         };

         while(!file.eof() && file.good())
         {
            s = ""; nl = false;
            while (c != '\t' && c != '\n' && c != ' ' && c!= '\r' && !file.eof() && file.good())
            {
               s.push_back(c);
               c = file.get();
            }
            while((c == '\t' || c == '\n' || c==' ' || c == '\r') && !file.eof() && file.good())
            {
               if (c == '\n' || c == '\r') nl = true;
               c = file.get();
            };

            if (s != "") {
               // try to convert to a number
               std::stringstream str(s);
               double d;
               str >> d;
               if(str.fail()) 
               {
                  // no number 
                  ll->append(ExprPtrT(new ExprString(s)));
               } 
               else
               {
                  if (d-int(d) == 0) 
                  { 
                     // integer
                     ll->append(ExprPtrT(new ExprInteger(int(d))));
                  } else {
                     // double
                     ll->append(ExprPtrT(new ExprReal(d)));
                  }
               }
            }

            if (nl || file.eof()) {
               if (ll->nargs() > 0 ) l->append(ll);
               ll = ExprPtrT(new ExprList());
            }
         }
         file.close();

         return l;
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 1) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};







/***********************************************************************

    Random Numbers 

************************************************************************/

//RandomInteger[], RandomInteger[s,e]

class ExprRandomInteger : public Expression
{
public: 
   ExprRandomInteger() : Expression() {};
   
   ExprRandomInteger(const ExprPtrT& s, const ExprPtrT& e) : Expression(s,e)
   {};
   
public: 
   EXPR_NAME_DECL()
   
   ExprPtrT evaluate(ExprScopeT* scope)
   {
      EXPR_EVAL_CHECK_SYNTAX()
      
      if (nargs() == 2)
      {
         ExprPtrT s = arg[0]->evaluate(scope);
         ExprPtrT e = arg[1]->evaluate(scope);
         if (s->numberQ() && e->numberQ())
         {
            double ds = double(*s);
            double de = double(*e);
            return ExprPtrT(new ExprInteger(random_number(ds, de)));
         };
         if (s == arg[0] && e == arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprRandomInteger(s,e));
      }
      
      return ExprPtrT(new ExprInteger(int(rand())));
   };
   
   bool toTypeQ(const std::type_info& ti) const 
   { 
      return (  ti == typeid(int) 
      || ti == typeid(double) 
      || ti == typeid(bool)
      );
   };
   
   operator double () const 
   {
      return int(*this);
   };
   
   operator int () const
   {
      if (nargs() == 0) return rand();
      EXPR_EVAL_ASSERT( arg[0]->numberQ() && arg[1]->numberQ(), RandomToNumber, this)
      double s = double(*arg[0]);
      double e = double(*arg[1]);
      return random_number(s,e);
      
      //is this necessary ?
   };

   operator bool () const
   {
      return true;
   };
   
   bool numberQ() { return true; };
   bool integerQ(){ return true; };
   bool realQ()   { return true; };
   
   ExprSyntaxErrorT check_syntax() const
   {
      if (nargs() != 0 && nargs() != 2) return IllegalArgumentNumber;
      else return NoSyntaxError;
   };
   
   inline int random_number(const double& s, const double& e) const
   {
      //return (e-s)*gsl_rng_uniform(random) + s;
      return int((e-s)*(rand()/(double) (RAND_MAX)) + s);
   }
   
   
};



// Random[], Random[s,e]

class ExprRandom : public Expression
{
   public:
      //static gsl_rng* random;  //  = gsl_rng_alloc(gsl_rng_default);

   public: 
      ExprRandom() : Expression() {};

      ExprRandom(const ExprPtrT& s, const ExprPtrT& e) : Expression(s,e)
      {};

   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         if (nargs() == 2)
         {
            ExprPtrT s = arg[0]->evaluate(scope);
            ExprPtrT e = arg[1]->evaluate(scope);

            if (s->numberQ() && e->numberQ())
            {
               double ds = double(*s);
               double de = double(*e);
               return ExprPtrT(new ExprReal(random_number(ds, de)));
            };
            if (s == arg[0] && e == arg[1]) return ExprPtrT(this);
            return ExprPtrT(new ExprRandom(s,e));
         }

         return ExprPtrT(new ExprReal(random_number(0.0, 1.0)));
      };

      bool toTypeQ(const std::type_info& ti) const 
      { 
         return (  ti == typeid(int) 
                || ti == typeid(double) 
                || ti == typeid(bool)
                );
      };

      operator double () const 
      {
         if (nargs() == 0) return random_number(0.0, 1.0);
         EXPR_EVAL_ASSERT( arg[0]->numberQ() && arg[1]->numberQ(), RandomToNumber, this)
         double s = double(*arg[0]);
         double e = double(*arg[1]);
         return random_number(s,e);
      };

      operator int () const
      {
         return int(double(*this));
      };

      operator bool () const
      {
         return true;
      };

      bool numberQ() { return true; };
      bool realQ()   { return true; };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 0 && nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };

      inline double random_number(const double& s, const double& e) const
      {
         //return (e-s)*gsl_rng_uniform(random) + s;
         return (e-s)*(rand()/(double) (RAND_MAX)) + s;
      }


};


class ExprSeed : public Expression
{
   public: 
      ExprSeed() : Expression() {};

      ExprSeed(const ExprPtrT& s) : Expression(s)
      {};

   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()
         
         if (nargs() == 0) { 

           timespec ts;
            
           #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
           clock_serv_t cclock;
           mach_timespec_t mts;
           host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
           clock_get_time(cclock, &mts);
           mach_port_deallocate(mach_task_self(), cclock);
           ts.tv_sec = mts.tv_sec;
           ts.tv_nsec = mts.tv_nsec;

           #else
           
            // random initialization with time !!!
            //srand ( time(NULL) );            
            //srand( clock() );
            //timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            //std::cout << "Seed time: " << ts.tv_nsec << std::endl;
            
            #endif
            
            srand( ts.tv_nsec );
            
         } else { //nargs ==1
            ExprPtrT seed = arg[0]->evaluate(scope);

            EXPR_EVAL_ASSERT(seed->integerQ() , RandomSeedNotInteger, this)

            //gsl_rng_set(ExprRandom::random, int(*seed));
            srand(int(*seed));
         }

         return ExprNullPtr();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (!(nargs() == 0 || nargs() ==1)) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};



/*********************************************************************

String Manipulation 

*********************************************************************/


class ExprToString : public Expression
{
   public: 
      ExprToString() : Expression() {};

      ExprToString(const ExprPtrT& str) : Expression(str)
      {};

   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT str = arg[0]->evaluate(scope);         
         if (str->stringQ()) return str;
         if (str->atomQ()) return ExprPtrT(new ExprString(str->print()));
         return ExprPtrT(new ExprToString(str));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 1) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};



/*****************************************************************************

List Manipulation 

*****************************************************************************/


class ExprRange : public Expression
{
   public: 
      ExprRange() : Expression() {};

      ExprRange(const ExprPtrT& n) : Expression(n)
      {};

      ExprRange(const ExprPtrT& s, const ExprPtrT& e) : Expression(s,e)
      {};

      ExprRange(const ExprPtrT& s, const ExprPtrT& e, const ExprPtrT& d) : Expression(s,e,d)
      {};


   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()


         ExprPtrT s = arg[0]->evaluate(scope);
         if (nargs() ==1) {
            if (s->numberQ()) {
               ExprPtrT l = ExprPtrT(new ExprList());
               for (int i=1; i <= int(*s); i++) {
                  l->append(ExprPtrT(new ExprInteger(i)));
               }
               return l;
            }  else {
               return ExprPtrT(new ExprRange(s));
            }
         }
         ExprPtrT e = arg[1]->evaluate(scope);
         if (nargs()==2) {
            if (s->integerQ() && e->numberQ()) {
               ExprPtrT l = ExprPtrT(new ExprList());
               for (int i=int(*s); i <= int(*e); i++) {
                  l->append(ExprPtrT(new ExprInteger(i)));
               }
               return l;
            } else if (s->numberQ() && e->numberQ()) {
               ExprPtrT l = ExprPtrT(new ExprList());
               for (double i=double(*s); i <= double(*e); i++) {
                  l->append(ExprPtrT(new ExprReal(i)));
               }
               return l;
            }  else {
               return ExprPtrT(new ExprRange(s,e));
            }
         }
         ExprPtrT d = arg[2]->evaluate(scope);
         if (s->integerQ() && e->numberQ() && d->integerQ()) {
            ExprPtrT l = ExprPtrT(new ExprList());
            for (int i=(*s); i <= int(*e); i+=int(*d)) {
               l->append(ExprPtrT(new ExprInteger(i)));
            }
            return l;
         } else if (s->numberQ() && e->numberQ() && d->numberQ()) {
               ExprPtrT l = ExprPtrT(new ExprList());
               for (double i=double(*s); i <= double(*e); i+=double(*d)) {
                  l->append(ExprPtrT(new ExprReal(i)));
               }
               return l;
         }
         return ExprPtrT(new ExprRange(s,e,d));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() <1 || nargs() > 3) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};




/*****************************************************************************

Special Functions 

*****************************************************************************/

class ExprHammingDistance : public Expression
{
public: 
   ExprHammingDistance() : Expression() {};
   
   ExprHammingDistance(const ExprPtrT& x, const ExprPtrT& y) : Expression(x,y)
   {};
   
public: 
   EXPR_NAME_DECL()
   
   ExprPtrT evaluate(ExprScopeT* scope)
   {
      EXPR_EVAL_CHECK_SYNTAX()
      
      ExprPtrT x = arg[0]->evaluate(scope);
      ExprPtrT y = arg[1]->evaluate(scope);
      if (x->numberQ() && y->numberQ())
      {
         int ix = int(*x);
         int iy = int(*y);
         
         return ExprPtrT(new ExprInteger(hamming_dist(ix,iy)));
      };
      if (x == arg[0] && y == arg[1]) return ExprPtrT(this);
      return ExprPtrT(new ExprHammingDistance(x,y));
   };
   
   bool toTypeQ(const std::type_info& ti) const 
   { 
      return (  ti == typeid(int) 
      || ti == typeid(double) 
      || ti == typeid(bool)
      );
   };
   
   operator double () const 
   {
      return int(*this);
   };
   
   operator int () const
   {
      if (nargs() == 0) return rand();
      EXPR_EVAL_ASSERT( arg[0]->numberQ() && arg[1]->numberQ(), NonNumberToIntg, this)
      int x = int(*arg[0]);
      int y = int(*arg[1]);
      return hamming_dist(x,y);
      
      //is this necessary ?
   };

   operator bool () const
   {
      return true;
   };
   
   bool numberQ() { return true; };
   bool integerQ(){ return true; };
   bool realQ()   { return true; };
   
   ExprSyntaxErrorT check_syntax() const
   {
      if (nargs() != 2) return IllegalArgumentNumber;
      else return NoSyntaxError;
   };
   
   inline int hamming_dist(const int& x, const int& y) const
   {
      int dist = 0;
      unsigned int val = x ^ y;
      while(val) {
         ++dist;
         val &= val-1;
      }
      return dist;
   }
};




#endif
