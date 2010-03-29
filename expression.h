/***************************************************************************
   expression.h   -  code expressions 
   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <typeinfo>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <deque>
#include <map>

#include <assert.h>


//#define EXPR_DEBUG(s1, s2) std::cout << "ExprDebug: " << s1 << " " << s2 << std::endl; std::cout.flush();
#define EXPR_DEBUG(s1, s2)

//#define EXPR_PTR_DEBUG(s1, s2) std::cout << "ExprDebug: " << s1 << " " << s2 << std::endl; std::cout.flush();
#define EXPR_PTR_DEBUG(s1, s2)



class ExpressionError
{
   std::string what_;
public:
   ExpressionError(const std::string w) : what_(w) {};
   std::string what() const
   {
      return what_;
   }
};

#define EXPR_EVAL_ASSERT(cond, err, where) if (!(cond)) { std::ostringstream str; str << "Expression Evaluation Error: " << int(err) << " = " << ExprEvalErrorDescription(err) << "\nError occured in Expression: " << where->print() << std::endl; /*assert(false);*/ throw ExpressionError(str.str()); };

#define EXPR_EVAL_CHECK_SYNTAX() ExprSyntaxErrorT synerr = check_syntax(); if (synerr != NoSyntaxError) { std::ostringstream str; str << "Expression Syntax Error in evaluation: " << int(synerr) << " = " << ExprSyntaxErrorDescription(synerr) << "\nError occured in Expression: " << this->print() << std::endl; /*assert(false);*/ throw ExpressionError(str.str()); };

#define EXPR_MAX_RECURSION  256
#define EXPR_MAX_SCOPELEVEL 512

#define EXPR_NAME_DECL() static const ExprNameT Name; ExprNameT name() const; 

#define EXPR_NAME_IMPL(expr, expname) const ExprNameT expr::Name = expname; ExprNameT expr::name() const { return Name; };

#define EXPR_BIOP_DECL(expr, expop, exptempl) typedef exptempl<expop> expr; template <> const ExprNameT expr::Name; template <> const std::string expr::OpSymbol; template<> ExprNameT expr::name() const; template <> const expop expr::op;

#define EXPR_BIOP_IMPL(expr, expop, expname, expsymb) template<> const ExprNameT expr::Name = expname; template<> ExprNameT expr::name() const { return Name; }; template<> const std::string expr::OpSymbol = expsymb; template<> const expop expr::op = expop();

#define EXPR_MATU_DECL(expr, expop, exptempl) typedef exptempl<expop> expr; template <> const ExprNameT expr::Name; template <> const expop expr::op; template<> ExprNameT expr::name() const;

#define EXPR_MATU_IMPL(expr, expop, expname) template<> const ExprNameT expr::Name = expname; template<> ExprNameT expr::name() const { return Name; };  template<> const expop expr::op = expop();


class Expression;
class ExpressionPtr;
class ExpressionScope;

typedef ExpressionScope                ExprScopeT;
typedef ExpressionPtr                  ExprPtrT;
typedef long                           ExprRefT;
typedef int                            ExprStatusT;

typedef std::string                    ExprNameT;
typedef uint                           ExprIndexT;
typedef std::deque<ExprPtrT>           ExprArgT;
typedef ExprArgT::iterator             ExprArgIteratorT;
typedef ExprArgT::const_iterator       ExprArgConstIteratorT;

enum ExprEvalErrorT
               { 
                  NoEvalError = 0, 
                  NonNumberToReal,
                  NonNumberToIntg,
                  NonStrgToStrg,
                  NonBoolToBool,

                  MaxRecursion,
                  MaxScope,

                  EvaluateAtOnAtom,
                  EvaluateAtIllegalArgs,

                  ExtractOutOfRange,
                  InsertOutOfRange,
                  ReplaceOutOfRange,

                  ReturnToGlobal,
                  BreakToGlobal,

                  DivisionByZero,

                  // Sim Expressions
                  IteratorExpectList,
                  IteratorExpectNumber,
                  IteratorLoop,

                  CreatorExpectList,
                  CreatorExpectNumber,

                  // extras
                  RandomToNumber,
                  RandomSeedNotInteger,

                  // Kernel
                  TypeMismatch,

                  // internal errors -> debugging
                  EvaluateBase,
                  RecursionUnderflow,
                  ScopeUnderflow,
                  OutOfArgRange,
                  SymbolNameOnNonSymbol,
                  EvaluationOnNoMatch,

                  // internal Sim
                  IteratorInternalError
               };

enum ExprSyntaxErrorT 
               { // Syntax errors evoked by check_syntax
                  NoSyntaxError = 0,
                  SyntaxError,

                  IllegalArgumentNumber,

                  PatternExpectSymbol,

                  SetExpectSymbol,
                  SetExpectFunctionOrSymbol,
                  DefineExpectSymbol,
                  DefineExpectFunctionOrSymbol,

                  TableExpectList,
                  TableExpectSymbol,

                  FunctionExpectSymbolList,

                  ModuleExpectSymbolList,
                  ModuleExpectBlock,

                  //sim 
                  IteratorExpectSymbol,
                  IteratorExpectIterationList,
                  IteratorSyntaxError,
                  CreatorExpectIterationList,
                  CreatorExpectSymbol,
                  CreatorSyntaxError,
               };

typedef std::map<ExprEvalErrorT, std::string>    ExprEvalErrorDescriptionT;
typedef std::map<ExprSyntaxErrorT, std::string>  ExprSyntaxErrorDescriptionT;

std::string ExprEvalErrorDescription(const ExprEvalErrorT& e);
std::string ExprSyntaxErrorDescription(const ExprSyntaxErrorT& e);

/*********************************************************************************

   Base Class Expression 

**********************************************************************************/
class Expression
{
   public:
      ExprArgT     arg;    // argument list
      ExprRefT     refs;   // number of ExpressionPointers to this Expression
      ExprStatusT  status; // status to detect infinite recursions

   public:
      //Constructors 
      Expression();
      Expression(const ExprIndexT& n);
      Expression(const ExprPtrT& arg1);
      Expression(const ExprPtrT& arg1, const ExprPtrT& arg2);
      Expression(const ExprPtrT& arg1, const ExprPtrT& arg2, const ExprPtrT& arg3);
      Expression(const ExprPtrT& arg1, const ExprPtrT& arg2, const ExprPtrT& arg3, const ExprPtrT& arg4);

      virtual ~Expression();

   public:
      virtual ExprNameT name() const = 0;
      virtual ExprIndexT nargs() const;

      virtual ExprPtrT evaluate(ExprScopeT* scope);
      virtual ExprSyntaxErrorT check_syntax() const;

      virtual void append(const ExprPtrT& e);
      virtual void prepend(const ExprPtrT& e);
      virtual void set(const ExprIndexT& i, const ExprPtrT& e);
      virtual void insert(const ExprIndexT& i, const ExprPtrT& e);
      virtual void clear();

      virtual std::string print() const;

      virtual void begin_evaluate();
      virtual void end_evaluate();

      // Type conversion
      virtual bool toTypeQ(const std::type_info& ti) const { return false; };

      virtual operator double () const;
      virtual operator int () const;
      virtual operator std::string () const;
      virtual operator bool () const;

      // Compare Expressions
      virtual bool equalQ(const ExprPtrT& rhs) const;

      // Pattern ordering
      virtual bool lessPatternQ(const ExprPtrT& rhs) const;

      // Pattern matching
      virtual bool matchQ(const ExprPtrT& expr) const;

      // Scoping Patterns
      virtual void protectScopePattern(ExprScopeT* scope) const;
      virtual void defineScopePattern(ExprScopeT* scope, const ExprPtrT& expr) const;

      // Properties
      virtual bool integerQ() const   { return false; };
      virtual bool realQ() const      { return false; };
      virtual bool numberQ() const    { return false; };
      virtual bool stringQ() const    { return false; };
      virtual bool boolQ() const      { return false; };
      virtual bool symbolQ() const    { return false; };
      virtual bool atomQ() const      { return false; };
      virtual bool listQ() const      { return false; };
      virtual bool functionQ() const  { return false; };
      virtual bool evalQ() const      { return false; };
      virtual bool blockQ() const     { return false; };
      virtual bool globalQ() const    { return false; };
      virtual bool allQ() const       { return false; };
      virtual bool automaticQ() const { return false; };
      virtual bool noneQ() const      { return false; };
      virtual bool sequenceQ() const  { return false; };

      virtual ExprIndexT depth() const { return 0; };

      virtual ExprNameT symbolname() const;
};


/*********************************************************************************

   Expression Pointer

**********************************************************************************/

class ExpressionPtr
{
public:

   Expression*   ptr;

   explicit ExpressionPtr();
   explicit ExpressionPtr(Expression* e);
   ~ExpressionPtr();

   ExpressionPtr(const ExpressionPtr& r);

   Expression* get();
   Expression* get() const;

   Expression* operator -> ();
   Expression* operator -> () const;
   Expression& operator * ();
   Expression& operator * () const;

   ExpressionPtr& operator = (const ExpressionPtr& rhs);
   ExpressionPtr& operator = (Expression* e);

   // equality of pointers !
   bool operator == (const ExpressionPtr& rhs) const;
   bool operator == (Expression* rhs) const;
   bool operator != (const ExpressionPtr& rhs) const;

   // Pattern ordering for ExpressionScope
   bool operator < (const ExpressionPtr& rhs) const;
};

/*********************************************************************************

   Fundamental Expressions

**********************************************************************************/
class ExprNull : public Expression 
{
   public:
      ExprNull();

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope);

      std::string print() const;
};

class ExprNoPattern : public Expression
{
   public:
      ExprNoPattern();

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope);

      bool lessPatternQ(const ExprPtrT& rhs) const;

      bool matchQ(const ExprPtrT& expr) const;
};

class ExprPattern : public Expression 
{
   public:
      ExprPattern(const ExprPtrT& s);

      ExprPtrT evaluate(ExprScopeT* scope);
      ExprSyntaxErrorT check_syntax() const;

      bool lessPatternQ(const ExprPtrT& rhs) const;

      bool matchQ(const ExprPtrT& expr) const;

      void protectScopePattern(ExprScopeT* scope) const;
      void defineScopePattern(ExprScopeT* scope, const ExprPtrT& expr) const;

      ExprNameT symbolname() const;

      static const ExprNameT Name;
      ExprNameT name() const;

      std::string print() const;
};


class ExprNoMatch : public Expression
{
   public:
      ExprNoMatch();

   public:
      EXPR_NAME_DECL()

      ExprPtrT evaluate(ExprScopeT* scope);

      std::string print() const;
};



// Pointer to Global Objects
ExprPtrT ExprNullPtr();
ExprPtrT ExprNoPatternPtr();
ExprPtrT ExprNoMatchPtr();

/*********************************************************************************

   Expression Scope

**********************************************************************************/
class ExpressionScope
{
public:
   typedef std::map<ExprPtrT, ExprPtrT>      PatternDefsT;
   typedef PatternDefsT::iterator            PatternDefsIter;
   typedef std::map<ExprNameT, PatternDefsT> DefsT; // optimize: ternary search tree instead of map!
   typedef DefsT::iterator                   DefsIter;
   typedef std::deque< DefsT >               DefsStackT;
   typedef DefsStackT::iterator              DefsStackIter;
   typedef uint                              ScopeLevelT;

public:

   DefsStackT     defs;
   DefsStackIter  top_defs;
   DefsStackIter  global_defs;
   ScopeLevelT    slevel;

public:
   ExpressionScope();

   void define(const ExprNameT& name, const ExprPtrT& expression);
   void define(const ExprNameT& name, const ExprPtrT& pattern, const ExprPtrT& expression);

   void define_local(const ExprNameT& name, const ExprPtrT& expression);
   void define_local(const ExprNameT& name, const ExprPtrT& pattern, const ExprPtrT& expression);

   ExprPtrT match(const ExprNameT& name) const;
   std::pair<ExprPtrT,ExprPtrT> match(const ExprNameT& name, const ExprPtrT& expr) const;

   void pop();
   void push();

   void clear();

   ScopeLevelT level() const;

   //inspection
   bool defined(const ExprNameT& name);
   bool defined(const ExprNameT& name, const ExprPtrT& pattern);

   std::string print_definitions(DefsStackIter it);
   std::string info();
};


/*********************************************************************************

   io

**********************************************************************************/
std::ostream& operator<<(std::ostream& os, Expression* e);
std::ostream& operator<<(std::ostream& os, const ExprPtrT& e);

#endif
