/**************************************************************************
   expression_basic.h   -  basic expressions for the parameter kernel

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation GÃ¶ttingen
   HU Berlin, BCCN GÃ¶ttingen & Berlin (2008)
****************************************************************************/
#ifndef EXPRESSION_BASIC_H
#define EXPRESSION_BASIC_H

#include <string>
#include <sstream>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <map>

#include "expression.h"

/***********************************************************************

   Atoms

************************************************************************/

//number
class ExprReal : public Expression
{
   public:
      double value;

   public:
      ExprReal() : Expression(), value(0.0) {};

      ExprReal(const double& r) : Expression(), value(r) {};

      ExprReal(const std::string& s) : Expression(),  value(atof(s.c_str())) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         std::stringstream str; str << value;
         return str.str();
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
         return value;
      };

      operator int () const
      {
         return int(value);
      };

      operator bool () const
      {
         return (value != 0.0);
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         if (name()== rhs->name()) return (value == ((ExprReal*)(rhs.get()))->value);
         else return false;
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         if (name() == rhs->name()) return (value < ((ExprReal*)(rhs.get()))->value);
         else return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name() && (value == ((ExprReal*)(rhs.get()))->value));
      };

      bool realQ() const   { return true; };
      bool numberQ() const { return true; };
      bool atomQ() const   { return true; };
};


class ExprInteger : public Expression
{
   public: 
      int value;

   public:
      ExprInteger() : Expression(), value(0) {};

      ExprInteger(const int& i) : Expression(), value(i) {};

      ExprInteger(const std::string& s) : Expression(), value(atoi(s.c_str())) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         std::stringstream str; str << value;
         return str.str();
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
         return double(value);
      };

      operator int () const
      {
         return value;
      };

      operator bool () const
      {
         return (value != 0);
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         if (name()== rhs->name()) return (value == ((ExprInteger*)(rhs.get()))->value);
         else return false;
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         if (name() == rhs->name()) return (value < ((ExprInteger*)(rhs.get()))->value);
         else return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name() && value == ((ExprInteger*)(rhs.get()))->value);
      };

      bool integerQ() const { return true; };
      bool numberQ() const  { return true; };
      bool atomQ() const    { return true; };

      ExprSyntaxErrorT check_syntax() const
      { 
         if (nargs() != 0) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};



class ExprString : public Expression
{
   public: 
      std::string value;

   public:
      ExprString() : Expression(), value("") {};

      ExprString(const std::string& s) : Expression(), value(s) 
      {
         if (*(value.begin()) == '"') value.erase(0,1);
         if (*(--value.end()) == '"') value.erase(value.size()-1,1);
      };

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         return '"' + value + '"';
      };

      bool toTypeQ(const std::type_info& ti) const 
      { 
         return (  (ti == typeid(std::string))
                || (ti == typeid(bool))
                );
      };

      operator std::string () const
      {
         return value;
      };

      operator bool () const
      {
         return (value != "");
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         if (name()== rhs->name()) return (value == ((ExprString*)(rhs.get()))->value);
         else return false;
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         if (name() == rhs->name()) return (value < ((ExprString*)(rhs.get()))->value);
         else return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name() && value == ((ExprString*)(rhs.get()))->value);
      };

      bool stringQ() const { return true; };
      bool atomQ() const   { return true; };
};



class ExprBool : public Expression
{
   public: 
      bool value;

   public:
      ExprBool() : Expression(), value(false) {};

      ExprBool(const bool& b) : Expression(), value(b) {};

      ExprBool(const std::string& s) : Expression(), value(false)
      {
         if (s == "True") value = true; 
      };

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         if (value) return std::string("True");
         else return std::string("False");
      };

      bool toTypeQ(const std::type_info& ti) const 
      { 
         return (ti == typeid(bool));
      };

      operator bool () const
      {
         return value;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         if (name()== rhs->name()) return (value ==  ((ExprBool*)(rhs.get()))->value);
         else return false;
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         if (name() == rhs->name()) return (value <  ((ExprBool*)(rhs.get()))->value);
         else return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name() && value ==  ((ExprBool*)(rhs.get()))->value);
      };

      bool boolQ() const { return true; };
      bool atomQ() const { return true; };
};



class ExprSymbol : public Expression
{
   public:
      ExprNameT symbol;

   public:
      ExprSymbol() : Expression(), symbol("") {};

      ExprSymbol(const ExprPtrT& s) : Expression(s), symbol("") {};

      ExprSymbol(const ExprNameT& s) : Expression(), symbol(s)
      {};

   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_DEBUG("ExprSymbol: evaluate():", print())
         ExprPtrT e(scope->match(symbol));
         if (e == ExprNoMatchPtr()) return ExprPtrT(this);

         EXPR_DEBUG("ExprSymbol: evaluate() matched to:", e)
//         ExprPtrT eo = ExprPtrT(this);
//         while (e != eo)
         e->begin_evaluate();
         EXPR_DEBUG("ExprSymbol: evaluate() status:", status)
         ExprPtrT r(e->evaluate(scope));
         e->end_evaluate();
         EXPR_DEBUG("ExprSymbol: evaluate() of " + symbol + " returns value:", r)
         return r;
      };

      std::string print() const
      {
         //if (nargs() > 1) return Name + "[" + arg[0]->print() + "]"; 
         return symbol;
      };

      ExprNameT symbolname() const
      {
         return symbol;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         if (name()== rhs->name()) return (symbolname() == rhs->symbolname());
         else return false;
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         if (name() == rhs->name()) return (symbolname() < rhs->symbolname());
         else return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name() && symbolname() == rhs->symbolname());
      };

      bool symbolQ() const { return true; };
      bool atomQ() const   { return true; };
};


/***********************************************************************

   Constants

************************************************************************/

class ExprPi : public Expression
{
   public:
      ExprPi() : Expression() {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         return Name;
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
         return 3.1415926535897932384;
      };

      operator int () const
      {
         return int(3.1415926535897932384);
      };

      operator bool () const
      {
         return true;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         return (name()== rhs->name());
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool realQ() const   { return true; };
      bool numberQ() const { return true; };
      bool atomQ() const   { return true; };
};


class ExprE : public Expression
{
   public:
      ExprE() : Expression() {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         return Name;
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
         return 2.7182818284590452354;
      };

      operator int () const
      {
         return int(2.7182818284590452354);
      };

      operator bool () const
      {
         return true;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         return (name()== rhs->name());
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool realQ() const   { return true; };
      bool numberQ() const { return true; };
      bool atomQ() const   { return true; };
};

class ExprGoldenRatio : public Expression
{
   public:
      ExprGoldenRatio() : Expression() {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         return Name;
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
         return 1.6180339887498948482;
      };

      operator int () const
      {
         return int(1.6180339887498948482);
      };

      operator bool () const
      {
         return true;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         return (name()== rhs->name());
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool realQ() const   { return true; };
      bool numberQ() const { return true; };
      bool atomQ() const   { return true; };
};


/***********************************************************************

   Special Symbols

************************************************************************/

class ExprAll : public Expression
{
   public:
      ExprAll() : Expression() {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         return Name;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool allQ() const   { return true; };
};


class ExprAutomatic : public Expression
{
   public:
      ExprAutomatic() : Expression() {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         return Name;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool automaticQ() const   { return true; };
};


class ExprNone : public Expression
{
   public:
      ExprNone() : Expression() {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         return ExprPtrT(this);
      };

      std::string print() const
      {
         return Name;
      };

      bool equalQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool lessPatternQ(const ExprPtrT& rhs) const
      {
         return (name() < rhs->name());
      };

      bool matchQ(const ExprPtrT& rhs) const
      {
         return (name() == rhs->name());
      };

      bool noneQ() const   { return true; };
};



/***********************************************************************

   Data Structures 

************************************************************************/

class ExprSequence : public Expression
{
   public: 
      ExprSequence() : Expression() {};

      ExprSequence(const ExprIndexT& l) : Expression(l) {};

   public:
      EXPR_NAME_DECL()

      ExprIndexT length()
      {
         return nargs();
      };

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         ExprPtrT e(new ExprSequence());
         for(ExprArgIteratorT i = arg.begin(); i != arg.end(); i++)
            e->append((*i)->evaluate(scope));
         for(ExprIndexT i = 0; i < length(); i++)
            if (e->arg[i] != arg[i]) return e;;
         return ExprPtrT(this);
      };

      bool sequenceQ() const { return true; };

      ExprIndexT depth() const
      {
         if (nargs() == 0) return 0;
         ExprArgConstIteratorT i = arg.begin();
         ExprIndexT d = (*i)->depth();
         ExprIndexT id;
         while (d != 0 && i != arg.end())
         {
            id = (*i)->depth();
            if (d>id) d = id;
            i++;
         };
         return d;
      };

      std::string print() const
      {
         std::stringstream str;
         for (ExprArgConstIteratorT it = arg.begin(); it != arg.end(); it++)
         {
            str << (*it)->print();
            if (it != (arg.end()-1)) str << ",";
         };
         return str.str();
      };
};


class ExprList : public Expression
{
   public: 
      ExprList() : Expression() {};

      ExprList(const ExprIndexT& l) : Expression(l) {};

   public:
      EXPR_NAME_DECL()

      ExprIndexT length()
      {
         return nargs();
      };

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         ExprPtrT e(new ExprList());
         for(ExprArgIteratorT i = arg.begin(); i != arg.end(); i++)
            e->append((*i)->evaluate(scope));
         for(ExprIndexT i = 0; i < length(); i++)
            if (e->arg[i] != arg[i]) return e;;
         return ExprPtrT(this);
      };

      bool listQ() const { return true; };

      ExprIndexT depth() const
      {
         if (nargs() == 0) return 1;
         ExprArgConstIteratorT i = arg.begin();
         ExprIndexT d = 1 + (*i)->depth();
         ExprIndexT id;
         while (d != 1 && i != arg.end())
         {
            id = 1 + (*i)->depth();
            if (d>id) d = id;
            i++;
         };
         return d;
      };

      std::string print() const
      {
         std::stringstream str;
         str << "{";
         for (ExprArgConstIteratorT it = arg.begin(); it != arg.end(); it++)
         {
            str << (*it)->print();
            if (it != arg.end()-1) str << ",";
         };
         str << "}";
         return str.str();
      };
};


class ExprTable : public Expression
{
   public: 
      ExprTable() : Expression() {};

      ExprTable(const ExprPtrT& body, const ExprPtrT& it) : Expression(body,it) {};

   public:
      EXPR_NAME_DECL()
   
      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT s = arg[1]->arg[1]->evaluate(scope);
         ExprPtrT e = arg[1]->arg[2]->evaluate(scope);
         ExprPtrT d = ExprPtrT(new ExprInteger(1));
         if (arg[1]->nargs()>3) d = arg[1]->arg[3]->evaluate(scope);

         if (s->integerQ() && e->integerQ() && d->integerQ())
         {
            ExprNameT iter = arg[1]->arg[0]->symbolname();
            ExprPtrT l(new ExprList());
            scope->push();
            for (int i = int(*s); i <= int(*e); i+=int(*d))
            {
               scope->define_local(iter, ExprPtrT(new ExprInteger(i)));
               l->append(arg[0]->evaluate(scope));
            };
            scope->pop();
            return l;
         } 
         if (s->numberQ() && e->numberQ() && d->numberQ())
         {
            ExprNameT iter =  arg[1]->arg[0]->symbolname();
            ExprPtrT l(new ExprList());
            scope->push();
            for (double i = double(*s); i <= double(*e); i+=double(*d))
            {
               scope->define_local(iter, ExprPtrT(new ExprReal(i)));
               l->append(arg[0]->evaluate(scope));
            };
            scope->pop();
            return l;
         };

         if (s ==arg[1]->arg[1] && e == arg[1]->arg[2] && d == arg[1]->arg[3]) return ExprPtrT(this);

         ExprPtrT i(new ExprList());
         i->append(arg[1]->arg[0]);
         i->append(s); i->append(e); i->append(d);
         return ExprPtrT(new ExprTable(arg[0], i));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         if (!(arg[1]->listQ() && (arg[1]->nargs() == 4 || arg[1]->nargs() ==3))) return TableExpectList;
         if (!(arg[1]->arg[0]->symbolQ())) return TableExpectSymbol;
         return NoSyntaxError;
      };
};

// Extract[List, Pos] or List[[pos]]
class ExprExtract : public Expression
{
   public: 
      ExprExtract() : Expression() {};

      ExprExtract(const ExprPtrT& l, const ExprPtrT& p) : Expression(l, p)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT l = arg[0]->evaluate(scope);
         ExprPtrT p = arg[1]->evaluate(scope);

         if (p->allQ() || p->integerQ() || p->listQ())
         {
            if (extract(l,p)) return l;
         }
         else if (p->sequenceQ())
         {
            ExprArgIteratorT pit = p->arg.begin();
            while (pit != p->arg.end())
            {
               if (!extract_sequence(l,p, pit)) break;
               pit++;
            };
            if (pit == p->arg.end()) return l;
            ExprPtrT np(new ExprSequence());
            while (pit != p->arg.end())
            {
               np->append(*pit);
               pit++;
            };
            return ExprPtrT(new ExprExtract(l, np));
         };
         if (l == arg[0] && p == arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprExtract(l, p));
      };

      std::string print() const
      {
         return arg[0]->print() + "[[" + arg[1]->print() + "]]";
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };

   private:
      bool extract(ExprPtrT& l, ExprPtrT& pi)
      {
         if (pi->allQ())
         {
            return true;
         }
         else if (pi->integerQ())
         {
            ExprIndexT i = int(*pi);
            EXPR_EVAL_ASSERT( i < l->nargs() , ExtractOutOfRange , this );
            l = l->arg[i];
            return true;
         }
         else if (pi->listQ())
         {
            for(ExprArgIteratorT j = pi->arg.begin(); j!= pi->arg.end(); j++)
               if (!((*j)->integerQ())) return false;

            ExprPtrT r(new ExprList());
            for(ExprArgIteratorT j = pi->arg.begin(); j!= pi->arg.end(); j++)
            {
               ExprIndexT i = int(*(*j));
               EXPR_EVAL_ASSERT( i < l->nargs() , ExtractOutOfRange , this );
               r->append(l->arg[i]);
            };
            l = r;
            return true;
         };
         return false;
      };

      bool extract_sequence(ExprPtrT& l, ExprPtrT& p, ExprArgIteratorT& pit)
      {
         if ((*pit)->integerQ() || (*pit)->listQ())
         {
            return extract(l, *pit);
         }
         else if ((*pit)->allQ())
         {
            pit++;
            if (pit == p->arg.end()) return true;
            if (!l->listQ()) return false;
            ExprPtrT ll(new ExprList());
            for (ExprArgIteratorT si = l->arg.begin(); si != l->arg.end(); si++)
            {
               if (!extract_sequence(*si, p, pit)) return false;
               ll->append(*si);
            };
            l = ll; 
            return true;
         };
         return false;
      };
};


class ExprAppend : public Expression
{
   public: 
      ExprAppend() : Expression() {};

      ExprAppend(const ExprPtrT& l, const ExprPtrT& a) : Expression(l,a) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT l = arg[0]->evaluate(scope);
         ExprPtrT a = arg[1]->evaluate(scope);

         if (l->listQ())
         {
            l->append(a);
            return l;
         };

         if (l == arg[0] && a == arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprAppend(l,a));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

class ExprPrepend : public Expression
{
   public: 
      ExprPrepend() : Expression() {};

      ExprPrepend(const ExprPtrT& l, const ExprPtrT& a) : Expression(l,a) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT l = arg[0]->evaluate(scope);
         ExprPtrT a = arg[1]->evaluate(scope);

         if (l->listQ())
         {
            l->prepend(a);
            return l;
         };

         if (l == arg[0] && a == arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprPrepend(l,a));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

class ExprReplace : public Expression
{
   public:
      ExprReplace() : Expression() {};

      ExprReplace(const ExprPtrT& l, const ExprPtrT& i, const ExprPtrT& a)
      : Expression(l,i,a)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT l = arg[0]->evaluate(scope);
         ExprPtrT i = arg[1]->evaluate(scope);
         ExprPtrT a = arg[2]->evaluate(scope);

         if (l->listQ() && i->integerQ())
         {
            int ii = int(*i);
            EXPR_EVAL_ASSERT(0 <= ii < l->nargs(), ReplaceOutOfRange, this)
            l->set(ii, a);
            return l;
         };

         if (l == arg[0] && i == arg[1] && a == arg[2]) return ExprPtrT(this);
         return ExprPtrT(new ExprReplace(l,i,a));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 3) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

class ExprInsert : public Expression
{
   public: 
      ExprInsert() : Expression() {};

      ExprInsert(const ExprPtrT& l, const ExprPtrT& i, const ExprPtrT& a) 
      : Expression(l,i,a) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT l = arg[0]->evaluate(scope);
         ExprPtrT i = arg[1]->evaluate(scope);
         ExprPtrT a = arg[2]->evaluate(scope);

         if (l->listQ() && i->integerQ())
         {
            int ii = int(*i);
            EXPR_EVAL_ASSERT(0 <= ii && ii < l->nargs(), InsertOutOfRange, this)
            l->insert(ii, a);
            return l;
         };

         if (l == arg[0] && i == arg[1] && a == arg[2]) return ExprPtrT(this);
         return ExprPtrT(new ExprInsert(l,i,a));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 3) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};


class ExprJoin : public Expression
{
   public: 
      ExprJoin() : Expression() {};

      ExprJoin(const ExprPtrT& l, const ExprPtrT& r) : Expression(l,r) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT l = arg[0]->evaluate(scope);
         ExprPtrT r = arg[1]->evaluate(scope);

         if (l->listQ() && r->listQ())
         {
            ExprPtrT n = ExprPtrT(new ExprList());
            for (ExprArgIteratorT i = l->arg.begin(); i != l->arg.end(); i++)
            {
               n->append(*i);
            }
            for (ExprArgIteratorT i = r->arg.begin(); i != r->arg.end(); i++)
            {
               n->append(*i);
            }
            return n;
         };

         if (l == arg[0] && r == arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprJoin(l,r));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};


class ExprLength : public Expression
{
   public: 
      ExprLength() : Expression() {};

      ExprLength(const ExprPtrT& l) : Expression(l) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT l = arg[0]->evaluate(scope);

         if (l->listQ())
         {
            return ExprPtrT(new ExprInteger(l->nargs()));
         };

         return ExprPtrT(new ExprLength(l));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 1) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};



/***********************************************************************

   Functions

************************************************************************/

// Function object
// Function[{Smybol["arg1"], Symbol["arg2"],...}, code]
class ExprFunction : public Expression
{
   public: 
      ExprFunction() : Expression() {};

      ExprFunction(const ExprPtrT& args, const ExprPtrT& code) : Expression(args,code)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         //EXPR_EVAL_CHECK_SYNTAX()
         return ExprPtrT(this);
      };

      ExprPtrT fcode() 
      {
         return arg[1];
      };

      ExprIndexT fargs()
      {
         return arg[0]->nargs();
      };

      ExprNameT argname(const ExprIndexT& i)
      {
         return arg[0]->arg[i]->symbolname();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         if (!(arg[0]->listQ())) return FunctionExpectSymbolList;
         for (ExprArgConstIteratorT it = arg[0]->arg.begin(); it != arg[0]->arg.end(); it++)
         {
            if (!((*it)->symbolQ())) return FunctionExpectSymbolList;
         };
         return NoSyntaxError;
      };

      bool functionQ() const { return true; };
};

// EvaluateAt[f, {arg1, arg2, arg3, ...}] or f[arg1, arg2, ...]
class ExprEvaluateAt : public Expression 
{
   public: 
      ExprEvaluateAt() : Expression() {};

      ExprEvaluateAt(const ExprPtrT& f, const ExprPtrT& a) : Expression(f,a)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_DEBUG("EvaluateAt:", arg[0]->print() + " " + arg[1]->print())
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT f = arg[0]->evaluate(scope);
         ExprPtrT a = arg[1]->evaluate(scope);

         EXPR_DEBUG("EvaluateAt: evaluated arguments:", f->print() + " " + a->print())

         if (f->symbolQ())
         {
            std::pair<ExprPtrT, ExprPtrT> m = scope->match(f->symbolname(), a);
            if (m.second == ExprNoMatchPtr()) return ExprPtrT(new ExprEvaluateAt(f,a));
            EXPR_DEBUG("EvaluateAt: m=", m.first->print() + " " + m.second->print())
            scope->push();
            m.first->defineScopePattern(scope, a);
            m.second->begin_evaluate();
            ExprPtrT r = m.second->evaluate(scope);
            m.second->end_evaluate();
            scope->pop();
            return r;
         };

         if (f->functionQ())
         {
            ExprFunction* ff = (ExprFunction*) f.get();
            EXPR_EVAL_ASSERT( a->listQ() && a->nargs() == ff->fargs(), EvaluateAtIllegalArgs, this );
            scope->push();
            for (ExprIndexT i = 0; i < ff->fargs(); i++)
               scope->define_local(ff->argname(i), a->arg[i]);
            ff->begin_evaluate();
            ExprPtrT r = ff->fcode()->evaluate(scope);
            ff->end_evaluate();
            scope->pop();
            return r;
         };

         if (f->atomQ()) // any non symbol atom
         {
            EXPR_EVAL_ASSERT(false, EvaluateAtOnAtom, this)
         };

         return ExprPtrT(new ExprEvaluateAt(f,a));
      };

      bool evalQ() const { return true; };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};



/***********************************************************************

    Scope Manipulation 

************************************************************************/

// Set[x,y]  (x = y)
class ExprSet : public Expression 
{
   public: 
      ExprSet() : Expression() {};

      ExprSet(const ExprPtrT& x, const ExprPtrT& y) : Expression(x,y)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         if (arg[0]->evalQ()) // set function Symbol[Pattern] = Expr
         {
            // EvaluateAt[Symbol, Pattern]
      //      ExprPtrT patterno = arg[0]->arg[0];
            ExprPtrT patternn = arg[0]->arg[1]->evaluate(scope);
      /*      ExprIndexT it = 0;
            while (patterno != patternn)
            {
               patterno = patternn; patternn = patternn->evaluate(scope);
               it++;
               EXPR_EVAL_ASSERT(it < EXPR_MAX_ITERATION, MaxEvalIteration, this)
            };
      */

            // protect pattern symbols and evaluate
            //scope->push();
            //pattern->protectScopePattern(scope);
      //      ExprPtrT expro = arg[1]
            ExprPtrT exprn = arg[1]->evaluate(scope);
      /*      ExprIndexT it = 0;
            while (expro != exprn)
            {
               expro = exprn; exprn = exprn->evaluate(scope);
               it++;
               EXPR_EVAL_ASSERT(it < EXPR_MAX_ITERATION, MaxEvalIteration, this)
            };
            //scope->pop();
      */
            scope->define(arg[0]->arg[0]->symbolname(), patternn, exprn);
            return ExprNullPtr();
         };

         // set symbol 
        // ExprPtrT so = arg[1]
         ExprPtrT sn = arg[1]->evaluate(scope);
        // ExprIndexT it = 0;
        /* while (so != sn)
         {
            so = sn; sn = sn->evaluate(scope);
            it++;
            EXPR_EVAL_ASSERT(it < EXPR_MAX_ITERATION, MaxEvalIteration, this)
         };
         */
         scope->define(arg[0]->symbolname(), sn);
         return ExprNullPtr();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         if (arg[0]->evalQ())
         {
             if (!(arg[0]->arg[0]->symbolQ())) return SetExpectSymbol;
             return NoSyntaxError;
         };
         if (!(arg[0]->symbolQ())) return SetExpectFunctionOrSymbol;

         return NoSyntaxError;
      };
};


// Define[x,y]  (x := y)
class ExprDefine : public Expression  {
   public: 
      ExprDefine() : Expression() {};

      ExprDefine(const ExprPtrT& x, const ExprPtrT& y) : Expression(x,y)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         if (arg[0]->evalQ()) // function definition
         {
            // EvaluateFunction[Symbol[x], pattern]
            ExprPtrT pattern = arg[0]->arg[1]->evaluate(scope);
            scope->define(arg[0]->arg[0]->symbolname(), pattern, arg[1]);
            return ExprNullPtr();
         };

         // variable definition
         scope->define(arg[0]->symbolname(), arg[1]);
         return ExprNullPtr();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         if (arg[0]->evalQ()) // function definition
         {
            // EvaluateFunction[Symbol[x], pattern]
            if (!(arg[0]->arg[0]->symbolQ())) return DefineExpectSymbol;
            return NoSyntaxError;
         };

         // variabel definition
         if (!(arg[0]->symbolQ())) return DefineExpectFunctionOrSymbol;
         return NoSyntaxError;
      };
};


/***********************************************************************

    Programming

************************************************************************/

enum ExprCodeActionTypeT { Return = 0, Break };

template<ExprCodeActionTypeT AT>
class ExprCodeActionT
{
      ExprPtrT expr_;

   public:
      ExprCodeActionT(const ExprPtrT& e) : expr_(e)
      {};

      ExprPtrT value() const
      { 
         return expr_;
      };

      ExprCodeActionTypeT type() const
      { 
         return AT;
      };
};


// expr1; expr2; ... exprn;
class ExprBlock : public Expression
{
   public: 
      ExprBlock() : Expression()
      {};

      ExprBlock(const ExprIndexT& n) : Expression(n)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         ExprArgT::iterator it = arg.begin();
         while (it != arg.end() )
         {
            EXPR_DEBUG("Block Evaluation: ", (*it)->print()) 
            it->get()->evaluate(scope);
            it++;
         };
         return ExprNullPtr();
      };

      bool blockQ() const { return true; };
};

// Global expressions: 
// expr1; expr2; ... exprn;
class ExprGlobal : public Expression
{
   public: 
      ExprGlobal() : Expression()
      {};

      ExprGlobal(const ExprIndexT& n) : Expression(n)
      {};

      ExprGlobal(const ExprPtrT& b) : Expression()
      {
         arg = b->arg;
      };

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         ExprArgT::iterator it = arg.begin();
         try
         {
            while (it != arg.end() )
            {
               EXPR_DEBUG("Global Evaluation:", (*it))
               it->get()->evaluate(scope);
               it++;
            };
         } 
         catch (const ExprCodeActionT<Return>& ca) 
         {
            EXPR_EVAL_ASSERT(false, ReturnToGlobal, this)
         }
         catch (const ExprCodeActionT<Break>& ca) 
         {
            EXPR_EVAL_ASSERT(false, BreakToGlobal, this)
         };;
         return ExprNullPtr();
      };

      bool globalQ() const { return true; };
};


class ExprReturn : public Expression
{
   public: 
      ExprReturn() : Expression()
      {};

      ExprReturn(const ExprPtrT& e) : Expression(e)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         if (nargs()>0) 
         {
            ExprPtrT e = arg[0]->evaluate(scope);
            throw ExprCodeActionT<Return>(e);
         }
         throw ExprCodeActionT<Return>(ExprNullPtr());
         return ExprNullPtr();
      };
};

class ExprBreak : public Expression
{
   public: 
      ExprBreak() : Expression()
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
          throw ExprCodeActionT<Break>(ExprNullPtr());
          return ExprNullPtr();
      };
};


//Module[{loc1, loc2,...}, body]
class ExprModule : public Expression
{
public:
   ExprModule() : Expression() {};

   ExprModule(const ExprPtrT& l, const ExprPtrT& c) : Expression(l,c)
   {};

public:
   EXPR_NAME_DECL()

   ExprPtrT evaluate(ExprScopeT* scope)
   {
      EXPR_EVAL_CHECK_SYNTAX()

      // define local symbols:
      scope->push();
      for (ExprArgIteratorT it = arg[0]->arg.begin(); it != arg[0]->arg.end(); it++)
          scope->define_local((*it)->symbolname(), *it);

      // evaluate
      try
      {
         arg[1]->evaluate(scope);
      } 
      catch (const ExprCodeActionT<Return>& ca)
      {
         scope->pop();
         return ca.value();
      };

      scope->pop();
      return ExprNullPtr();
   };

   ExprSyntaxErrorT check_syntax() const
   {
      if (nargs() != 2) return IllegalArgumentNumber;
      if (!(arg[0]->listQ())) return ModuleExpectSymbolList;
      for (ExprArgConstIteratorT i = arg[0]->arg.begin(); i != arg[0]->arg.end(); i++)
          if (!((*i)->symbolQ())) return ModuleExpectSymbolList;
      if (!(arg[1]->blockQ())) return ModuleExpectBlock;
      return NoSyntaxError;
   };
};



/***********************************************************************

    Flow Control

************************************************************************/

// If[cond, true expr, false expr]
class ExprIf : public Expression
{
   public:
      ExprIf() : Expression() {};

      ExprIf(const ExprPtrT& cond, const ExprPtrT& t) : Expression(cond, t)
      {
         append(ExprNullPtr());
      };

      ExprIf(const ExprPtrT& cond, const ExprPtrT& t, const ExprPtrT& f) : Expression(cond, t, f)
      {};

   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT cond = arg[0]->evaluate(scope);

         if (cond->toTypeQ(typeid(bool)))
         {
            if (bool(*cond)) return arg[1]->evaluate(scope);
            else 
            {
               if (nargs()>2) return arg[2]->evaluate(scope);
               else return ExprNullPtr();
            }
         };
         if (nargs()==2) return ExprPtrT(new ExprIf(cond, arg[1]));
         return ExprPtrT(new ExprIf(cond, arg[1], arg[2]));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2 && nargs() != 3) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};


//For[init, cond, inc, body]
class ExprFor : public Expression
{
   public: 
      ExprFor() : Expression() {};

      ExprFor(const ExprPtrT& init, const ExprPtrT& cond, const ExprPtrT& inc, const ExprPtrT& body)
      : Expression(init, cond, inc, body)
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT init = arg[0]->evaluate(scope);
         ExprPtrT cond = arg[1]->evaluate(scope);

         if (!(cond->toTypeQ(typeid(bool)))) return ExprNullPtr();
         while (bool(*cond))
         {
            try {
               ExprPtrT body = arg[3]->evaluate(scope);
            }
            catch (const ExprCodeActionT<Break>& ca)
            {
               return ExprNullPtr();
            }
            catch (const ExprCodeActionT<Return>& ca)
            {
               return ca.value();
            };

            ExprPtrT inc = arg[2]->evaluate(scope);
            cond = arg[1]->evaluate(scope);

            if (!(cond->toTypeQ(typeid(bool)))) return ExprNullPtr();
         };
         return ExprNullPtr();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 4) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};





/***********************************************************************

   Predefined Functions

************************************************************************/
template<typename OP>
class ExprMathUnaryOp : public Expression
{
   private:
      static const OP op;

   public:
      ExprMathUnaryOp() : Expression() {};

      ExprMathUnaryOp(const ExprPtrT& x) : Expression(x) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);

         if (x->listQ())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            for (ExprArgIteratorT it = x->arg.begin(); it != x->arg.end(); it++)
            {
               e = ExprPtrT(new ExprMathUnaryOp(*it));
               l->append(e->evaluate(scope));
            };
            return l;
         };
         if (x->numberQ()) return ExprPtrT(new ExprReal(op(double(*x))));
         if (x == arg[0]) return ExprPtrT(this);
         return ExprPtrT(new ExprMathUnaryOp(x));
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 1) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};


template<typename OP>
class ExprMathBinaryOp : public Expression
{
   private:
      static const OP op;
      static const std::string OpSymbol; 

   public:
      ExprMathBinaryOp() : Expression() {};

      ExprMathBinaryOp(const ExprPtrT& x, const ExprPtrT& y) : Expression(x,y) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);
         ExprPtrT y = arg[1]->evaluate(scope);

         if (x->atomQ() && y->listQ())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            for (ExprArgIteratorT it = y->arg.begin(); it != y->arg.end(); it++)
            {
               e = ExprPtrT(new ExprMathBinaryOp(x, *it));
               l->append(e->evaluate(scope));
            };
            return l;
         };
         if (x->listQ() && y->atomQ())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            for (ExprArgIteratorT it = x->arg.begin(); it != x->arg.end(); it++)
            {
               e = ExprPtrT(new ExprMathBinaryOp(*it, y));
               l->append(e->evaluate(scope));
            };
            return l;
         };
         if (x->listQ() && y->listQ() && x->nargs() == y->nargs())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            ExprArgIteratorT xit = x->arg.begin();
            for (ExprArgIteratorT yit = y->arg.begin(); yit != y->arg.end(); yit++)
            {
               e = ExprPtrT(new ExprMathBinaryOp(*xit, *yit));
               l->append(e->evaluate(scope));
               xit++;
            };
            return l;
         };

         if (x->integerQ() && y->integerQ()) return ExprPtrT(new ExprInteger(op(int(*x),int(*y))));
         if (x->numberQ() && y->numberQ()) return ExprPtrT(new ExprReal(op(double(*x),double(*y))));
         if (x == arg[0] && y== arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprMathBinaryOp(x,y));
      };

      std::string print() const
      {
         return std::string("(") + arg[0]->print() + OpSymbol + arg[1]->print() + ")";
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

/*
struct ExprPlusOp
{
   template<typename T>
   T operator () (const T& x, const T&y) const
   {
      return x+y;
   }
};
EXPR_BIOP_DECL(ExprPlus, ExprPlusOp, ExprMathBinaryOp)
template <> const ExprPlusOp ExprPlus::op; 
*/

// this allows not only for number but also for string + string operations
class ExprPlus : public Expression
{
   public:
      ExprPlus() : Expression() {};

      ExprPlus(const ExprPtrT& x, const ExprPtrT& y) : Expression(x,y) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);
         ExprPtrT y = arg[1]->evaluate(scope);

         if (x->atomQ() && y->listQ())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            for (ExprArgIteratorT it = y->arg.begin(); it != y->arg.end(); it++)
            {
               e = ExprPtrT(new ExprPlus(x, *it));
               l->append(e->evaluate(scope));
            };
            return l;
         };
         if (x->listQ() && y->atomQ())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            for (ExprArgIteratorT it = x->arg.begin(); it != x->arg.end(); it++)
            {
               e = ExprPtrT(new ExprPlus(*it, y));
               l->append(e->evaluate(scope));
            };
            return l;
         };
         if (x->listQ() && y->listQ() && x->nargs() == y->nargs())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            ExprArgIteratorT xit = x->arg.begin();
            for (ExprArgIteratorT yit = y->arg.begin(); yit != y->arg.end(); yit++)
            {
               e = ExprPtrT(new ExprPlus(*xit, *yit));
               l->append(e->evaluate(scope));
               xit++;
            };
            return l;
         };

         if (x->integerQ() && y->integerQ()) return ExprPtrT(new ExprInteger(int(*x) + int(*y)));
         if (x->numberQ() && y->numberQ()) return ExprPtrT(new ExprReal(double(*x) +double(*y)));
         if (x->stringQ() && y->stringQ()) return ExprPtrT(new ExprString(std::string(*x) + std::string(*y)));
         if (x == arg[0] && y== arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprPlus(x,y));
      };

      std::string print() const
      {
         return std::string("(") + arg[0]->print() + "+" + arg[1]->print() + ")";
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};




struct ExprSubtractOp
{
   template<typename T>
   T operator () (const T& x, const T&y) const
   {
      return x-y;
   }
};
EXPR_BIOP_DECL(ExprSubtract, ExprSubtractOp, ExprMathBinaryOp)

struct ExprMultiplyOp
{
   template<typename T>
   T operator () (const T& x, const T& y) const
   {
      return x*y;
   };
};
EXPR_BIOP_DECL(ExprMultiply, ExprMultiplyOp, ExprMathBinaryOp)

class ExprDivide : public Expression
{
   public:
      ExprDivide() : Expression() {};

      ExprDivide(const ExprPtrT& x, const ExprPtrT& y) : Expression(x, y) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);
         ExprPtrT y = arg[1]->evaluate(scope);

         if (x->atomQ() && y->listQ())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            for (ExprArgIteratorT it = y->arg.begin(); it != y->arg.end(); it++)
            {
               e = ExprPtrT(new ExprDivide(x, *it));
               l->append(e->evaluate(scope));
            };
            return l;
         };
         if (x->listQ() && y->atomQ())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            for (ExprArgIteratorT it = x->arg.begin(); it != x->arg.end(); it++)
            {
               e = ExprPtrT(new ExprDivide(*it, y));
               l->append(e->evaluate(scope));
            };
            return l;
         };
         if (x->listQ() && y->listQ() && x->nargs() == y->nargs())
         {
            ExprPtrT l(new ExprList());
            ExprPtrT e;
            ExprArgIteratorT xit = x->arg.begin();
            for (ExprArgIteratorT yit = y->arg.begin(); yit != y->arg.end(); yit++)
            {
               e = ExprPtrT(new ExprDivide(*xit, *yit));
               l->append(e->evaluate(scope));
               xit++;
            };
            return l;
         };
         if (x->numberQ() && y->numberQ()) 
         {
            double yy = double(*y);
            EXPR_EVAL_ASSERT( yy != 0, DivisionByZero, this)
            return ExprPtrT(new ExprReal(double(*x)/yy));
         };
         if (x == arg[0] && y== arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprDivide(x,y));
      };

      std::string print() const
      {
         return std::string("(") + arg[0]->print() + "/" + arg[1]->print() + ")";
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

struct ExprModOp
{
   template<typename T>
   T operator () (const T& x, const T& y) const
   {
      return x%y;
   };

   double operator () (const double& x, const double& y) const
   {
      return fmod(x,y);
   };
};
EXPR_BIOP_DECL(ExprMod, ExprModOp, ExprMathBinaryOp)

struct ExprPowerOp
{
   template<typename T>
   T operator () (const T& x, const T& y) const
   {
      return pow(x, y);
   };

   int operator () (const int& x, const int& y) const
   {
      return int(pow(double(x), y));
   };
};
EXPR_BIOP_DECL(ExprPower, ExprPowerOp, ExprMathBinaryOp)


// Unary Minus
class ExprMinus : public Expression
{
   public:
      ExprMinus() : Expression() {};

      ExprMinus(const ExprPtrT& x) : Expression(x) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);

         if (x->integerQ()) return ExprPtrT(new ExprInteger(-int(*x)));
         if (x->numberQ()) return ExprPtrT(new ExprReal(-double(*x)));
         if (x == arg[0]) return ExprPtrT(this);
         return ExprPtrT(new ExprMinus(x));
      };

      std::string print() const
      {
         return std::string("-") + arg[0]->print();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 1) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};


struct ExprSinOp
{
   template<typename T>
   T operator () (const T& x) const
   {
      return sin(x);
   };
};
EXPR_MATU_DECL(ExprSin, ExprSinOp, ExprMathUnaryOp)


struct ExprCosOp
{
   template<typename T>
   T operator () (const T& x) const
   {
      return cos(x);
   };
};
EXPR_MATU_DECL(ExprCos, ExprCosOp, ExprMathUnaryOp)

struct ExprTanOp
{
   template<typename T>
   T operator () (const T& x) const
   {
      return tan(x);
   };
};
EXPR_MATU_DECL(ExprTan, ExprTanOp, ExprMathUnaryOp)

struct ExprSinhOp
{
   template<typename T>
   T operator () (const T& x) const
   {
      return sinh(x);
   };
};
EXPR_MATU_DECL(ExprSinh, ExprSinhOp, ExprMathUnaryOp)

struct ExprCoshOp
{
   template<typename T>
   T operator () (const T& x) const
   {
      return cosh(x);
   };
};
EXPR_MATU_DECL(ExprCosh, ExprCoshOp, ExprMathUnaryOp)

struct ExprExpOp
{
   template<typename T>
   T operator () (const T& x) const
   {
      return exp(x);
   };
};
EXPR_MATU_DECL(ExprExp, ExprExpOp, ExprMathUnaryOp)

struct ExprLogOp
{
   template<typename T>
   T operator () (const T& x) const
   {
      return log(x);
   };
};
EXPR_MATU_DECL(ExprLog, ExprLogOp, ExprMathUnaryOp)


class ExprNot : public Expression
{
   public:
      ExprNot() : Expression() {};

      ExprNot(const ExprPtrT& x) : Expression(x) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);

         if (x->toTypeQ(typeid(bool))) return ExprPtrT(new ExprBool(!bool(*x)));
         if (x == arg[0]) return ExprPtrT(this);
         return ExprPtrT(new ExprNot(x));
      };

      std::string print() const
      {
         return std::string("!") + arg[0]->print();
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 1) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

template <typename OP>
class ExprBoolBinaryOp : public Expression
{
   private:
      static const OP op;
      static const std::string OpSymbol;

   public:
      ExprBoolBinaryOp() : Expression() {};

      ExprBoolBinaryOp(const ExprPtrT& x, const ExprPtrT& y) : Expression(x,y) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);
         ExprPtrT y = arg[1]->evaluate(scope);

         if (x->toTypeQ(typeid(bool)) && y->toTypeQ(typeid(bool))) 
            return ExprPtrT(new ExprBool(op(bool(*x),bool(*y))));
         if (x == arg[0] && y == arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprBoolBinaryOp(x,y));
      };

      std::string print() const
      {
         return std::string("(") + arg[0]->print() + OpSymbol + arg[1]->print() + ")";
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

struct ExprAndOp
{
   bool operator () (bool x, bool y) const
   {
      return x && y;
   };
};
EXPR_BIOP_DECL(ExprAnd, ExprAndOp, ExprBoolBinaryOp)


struct ExprOrOp
{
   bool operator () (bool x, bool y) const
   {
      return x || y;
   };
};
EXPR_BIOP_DECL(ExprOr, ExprOrOp, ExprBoolBinaryOp)


template<typename OP>
class ExprComparisonOp : public Expression
{
   private:
      static const OP op;
      static const std::string OpSymbol;

   public:
      ExprComparisonOp() : Expression() {};

      ExprComparisonOp(const ExprPtrT& x, const ExprPtrT& y) : Expression(x,y) 
      {};

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         EXPR_EVAL_CHECK_SYNTAX()

         ExprPtrT x = arg[0]->evaluate(scope);
         ExprPtrT y = arg[1]->evaluate(scope);

         if (x->integerQ() && y->integerQ()) 
            return ExprPtrT(new ExprBool(op(int(*x), int(*y))));
         if (x->numberQ() && y->numberQ()) 
            return ExprPtrT(new ExprBool(op(double(*x), double(*y))));
         if (x->stringQ() && y->stringQ()) 
            return ExprPtrT(new ExprBool(op(std::string(*x),std::string(*y))));
         if (x == arg[0] && y == arg[1]) return ExprPtrT(this);
         return ExprPtrT(new ExprComparisonOp(x,y));
      };

      std::string print() const
      {
         return std::string("(") + arg[0]->print() + OpSymbol + arg[1]->print() + ")";
      };

      ExprSyntaxErrorT check_syntax() const
      {
         if (nargs() != 2) return IllegalArgumentNumber;
         else return NoSyntaxError;
      };
};

struct ExprGreaterOp
{
   template <typename T> 
   bool operator () (const T& x, const T& y) const
   {
      return x > y;
   };
};
EXPR_BIOP_DECL(ExprGreater, ExprGreaterOp, ExprComparisonOp)

struct ExprLessOp
{
   template <typename T> 
   bool operator () (const T& x, const T& y) const
   {
      return x < y;
   };
};
EXPR_BIOP_DECL(ExprLess, ExprLessOp, ExprComparisonOp)


struct ExprEqualOp
{
   template <typename T> 
   bool operator () (const T& x, const T& y) const
   {
      return x == y;
   };
};
EXPR_BIOP_DECL(ExprEqual, ExprEqualOp, ExprComparisonOp)

struct ExprNotEqualOp
{
   template <typename T> 
   bool operator () (const T& x, const T& y) const
   {
      return x != y;
   };
};
EXPR_BIOP_DECL(ExprNotEqual, ExprNotEqualOp, ExprComparisonOp)


struct ExprGreaterEqualOp
{
   template <typename T> 
   bool operator () (const T& x, const T& y) const
   {
      return x >= y;
   };
};
EXPR_BIOP_DECL(ExprGreaterEqual, ExprGreaterEqualOp, ExprComparisonOp)


struct ExprLessEqualOp
{
   template <typename T> 
   bool operator () (const T& x, const T& y) const
   {
      return x <= y;
   };
};
EXPR_BIOP_DECL(ExprLessEqual, ExprLessEqualOp, ExprComparisonOp)

/***********************************************************************

    IO

************************************************************************/
#define EXPR_OUT std::cout
#define EXPR_ENDL std::endl

// Print[expr]
class ExprPrint : public Expression
{
   public: 
      ExprPrint() : Expression() {};

      ExprPrint(const ExprPtrT& p) : Expression(p)
      {};

   public: 
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope)
      {
         std::stringstream str;
         for (ExprArgIteratorT it = arg.begin(); it != arg.end(); it++)
         {
            str << ((*it)->evaluate(scope));
         };
         EXPR_OUT << str.str() << EXPR_ENDL;
         return ExprNullPtr();
      };
};

// extension: Plot etc...
// could also communicate with Mathematica here !




/***********************************************************************

    Type Conversions

************************************************************************/

template<typename T> 
std::string typeName()
{
   return std::string(typeid(T).name());
};

template<typename T> 
std::string toExprString(const T& val)
{
   std::ostringstream str;
   str << val;
   return str.str();
};
std::string toExprString(const std::string& val);
std::string toExprString(const char* val);

template<typename T> ExprPtrT 
ValueToExpr(const T& val)
{
   std::ostringstream str; 
   str << "Expression Error: No rule to convert " << toExprString(val);
   str << " of type " << typeName<T> << " to Expression" << std::endl;
   throw ExpressionError(str.str());
   return ExprNullPtr();
};
ExprPtrT ValueToExpr(const int& val);
ExprPtrT ValueToExpr(const double& val);
ExprPtrT ValueToExpr(const bool& val);
ExprPtrT ValueToExpr(const std::string& val);
ExprPtrT ValuetoExpr(const char* val);


template<typename T> 
std::string ExprToValueErrorDescription(const ExprPtrT& e)
{
   std::ostringstream str; 
   str << "Expression Error: Typemismatch:\nCannot convert: " << e;
   str << " to type " << typeName<T>();
   return str.str();
};

void ExprToValue(const ExprPtrT& e, long int& val);
void ExprToValue(const ExprPtrT& e, unsigned long& val);
void ExprToValue(const ExprPtrT& e, short& val);
void ExprToValue(const ExprPtrT& e, unsigned int& val);
void ExprToValue(const ExprPtrT& e, short unsigned int& val);
void ExprToValue(const ExprPtrT& e, float& val);
template<typename T> void ExprToValue(const ExprPtrT& e, T& val)
{
   if (!(e->toTypeQ(typeid(T)))) throw ExpressionError(ExprToValueErrorDescription<T>(e));
   val = T(*e);
};



#endif
