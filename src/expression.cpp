/***************************************************************************
   expression.cpp   -  code expressions 
   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#include "expression.h"

/*********************************************************************************

   Expression 

**********************************************************************************/
Expression::Expression() : refs(0), status(0)
{
//   EXPR_DEBUG("Expression Constructor", "Expression()")
};

Expression::Expression(const ExprIndexT& n) : refs(0), status(0)
{
//   EXPR_DEBUG("Expression Constructor: Expression(n)", n)
   arg.resize(n, ExprNullPtr());
};

Expression::Expression(const ExprPtrT& arg1) : refs(0), status(0)
{
//   EXPR_DEBUG("Expression Constructor: Expression(arg1)", arg1)
   append(arg1);
};

Expression::Expression(const ExprPtrT& arg1, const ExprPtrT& arg2) : refs(0), status(0)
{
//   EXPR_DEBUG("Expression Constructor: Expression(arg1, arg2)", "")
//   EXPR_DEBUG(arg1, arg2)
   append(arg1); append(arg2);
};

Expression::Expression(const ExprPtrT& arg1, const ExprPtrT& arg2, const ExprPtrT& arg3) 
: refs(0), status(0)
{
//   EXPR_DEBUG("Expression Constructor: Expression(arg1, arg2, arg3)", "")
//   EXPR_DEBUG(arg1, arg2)
   append(arg1); append(arg2); append(arg3);
};

Expression::Expression(const ExprPtrT& arg1, const ExprPtrT& arg2, const ExprPtrT& arg3, const ExprPtrT& arg4) : refs(0), status(0)
{
//   EXPR_DEBUG("Expression Constructor: Expression(arg1, arg2,arg3, arg4)", "")
//   EXPR_DEBUG(arg1, arg2)
   append(arg1); append(arg2); append(arg3); append(arg4);
};


Expression::~Expression()
{
//   EXPR_DEBUG("Expression Destructor", "")
   if (nargs()>0) arg.clear();
};

ExprIndexT Expression::nargs() const
{
   return arg.size();
};

ExprPtrT Expression::evaluate(ExprScopeT* scope) 
{ 
   EXPR_EVAL_ASSERT(false, EvaluateBase, this); 
   return ExprNullPtr(); 
};

ExprSyntaxErrorT Expression::check_syntax() const
{
   return NoSyntaxError;
};

void Expression::append(const ExprPtrT& e)
{
   arg.push_back(e);
};

void Expression::prepend(const ExprPtrT& e)
{
   arg.push_front(e);
};

void Expression::set(const ExprIndexT& i, const ExprPtrT& e)
{
   EXPR_EVAL_ASSERT( 0<= i < arg.size(), OutOfArgRange, this)
   arg[i] = e;
};

void Expression::insert(const ExprIndexT& i, const ExprPtrT& e)
{
   EXPR_EVAL_ASSERT( 0 <= i <= arg.size(), OutOfArgRange, this)
   ExprArgIteratorT it = arg.begin() + i;
   arg.insert(it, e);
};

void Expression::clear() 
{
   for (int i = 0; i<nargs(); i++)
   {
      arg[i]->clear();
   };
   arg.clear();
};

std::string Expression::print() const
{
   std::string s = "";
   for (ExprArgConstIteratorT i = arg.begin(); i != arg.end(); i++) 
   {
      s = s + (*i)->print();
      if (i != (arg.end()-1)) s = s + ",";
   };
   return name() + "[" + s + "]"; 
};

Expression::operator double () const 
{ 
   EXPR_EVAL_ASSERT( false , NonNumberToReal, this);
};

Expression::operator int () const 
{ 
   EXPR_EVAL_ASSERT( false , NonNumberToIntg, this);
};

Expression::operator std::string () const 
{ 
   EXPR_EVAL_ASSERT( false , NonStrgToStrg, this);
};

Expression::operator bool () const 
{ 
   EXPR_EVAL_ASSERT( false , NonBoolToBool, this);
};


// Compare Expressions
bool Expression::equalQ(const ExprPtrT& rhs) const
{
   if (name() != rhs->name()) return false;
   if (nargs() != rhs->nargs()) return false;
   for (ExprIndexT i = 0; i < nargs(); i++)
   {
      if (!(arg[i]->equalQ(rhs->arg[i]))) return false;
   };
   return true;
};


// Pattern ordering for definitions in scope
// ExprPattern > other > ExprNoPatter
bool Expression::lessPatternQ(const ExprPtrT& rhs) const
{
   if (name() < rhs->name()) return true;
   if (name() > rhs->name()) return false;
   // name() == rhs.name()
   if (nargs() < rhs->nargs()) return true;
   if (nargs() > rhs->nargs()) return false;
   // nargs() == rhs.nargs()
   for (ExprIndexT i = 0; i < nargs(); i++)
   {
      if (arg[i]->lessPatternQ(rhs->arg[i])) return true;
      if (rhs->arg[i]->lessPatternQ(arg[i])) return false;
   };
   // equal patterns
   return false; 
};

// Pattern matching
bool Expression::matchQ(const ExprPtrT& expr) const
{
   if (name() != expr->name()) return false;
   if (nargs() != expr->nargs()) return false;
   for (ExprIndexT i = 0; i < nargs(); i++)
   {
      if (!(arg[i]->matchQ(expr->arg[i]))) return false;
   };
   return true;
};

void Expression::protectScopePattern(ExprScopeT* scope) const
{
   for (ExprArgConstIteratorT it = arg.begin(); it != arg.end(); it++)
   {
      (*it)->protectScopePattern(scope);
   };
};

void Expression::defineScopePattern(ExprScopeT* scope, const ExprPtrT& expr) const
{
   // assume expr matches pattern !
   EXPR_DEBUG("defineScopePattern: ", this->print() + " " + expr->print())

   ExprArgConstIteratorT eit = (expr->arg).begin();
   ExprArgConstIteratorT pit = arg.begin();

   while (pit != arg.end())
   {
      (*pit)->defineScopePattern(scope, (*eit));
      eit++;
      pit++;
   };
};

ExprNameT Expression::symbolname() const
{
   EXPR_EVAL_ASSERT(false, SymbolNameOnNonSymbol, this)
   return ExprNameT();
};


void Expression::begin_evaluate()
{
   status++;
   EXPR_EVAL_ASSERT(status <= EXPR_MAX_RECURSION, MaxRecursion, this)
};

void Expression::end_evaluate()
{
   status--;
   EXPR_EVAL_ASSERT(0 <= status, RecursionUnderflow, this);
};



/****************************************************************************

      Fundamental Expressions

*****************************************************************************/

EXPR_NAME_IMPL(ExprNull, "Null")

ExprNull::ExprNull() : Expression() {};

ExprPtrT ExprNull::evaluate(ExprScopeT* scope)
{
   return ExprPtrT(this);
};

std::string ExprNull::print() const
{
   return name();
};


EXPR_NAME_IMPL(ExprNoPattern, "NoPattern")

ExprNoPattern::ExprNoPattern() : Expression() {};

ExprPtrT ExprNoPattern::evaluate(ExprScopeT* scope)
{
   return ExprPtrT(this);
};

// NoPattern[] < other (except for other == NoPattern[])
bool ExprNoPattern::lessPatternQ(const ExprPtrT& rhs) const
{
   if (ExprNull::Name == rhs->name()) return false;
   else return true;
};

// matches only itself 
bool ExprNoPattern::matchQ(const ExprPtrT& expr) const
{
   return (ExprNoPattern::Name == expr->name());
};

EXPR_NAME_IMPL(ExprPattern, "Pattern")

ExprPattern::ExprPattern(const ExprPtrT& s) : Expression(s) {};

ExprPtrT ExprPattern::evaluate(ExprScopeT* scope)
{
   EXPR_EVAL_CHECK_SYNTAX()
   return ExprPtrT(this);
};

ExprSyntaxErrorT ExprPattern::check_syntax() const
{
   if (nargs() != 1) return IllegalArgumentNumber; 
   if (!(arg[0]->symbolQ())) return PatternExpectSymbol;
   return NoSyntaxError;
};

// other < Pattern[...]
bool ExprPattern::lessPatternQ(const ExprPtrT& rhs) const
{
   return false;
};

// matches every expression
bool ExprPattern::matchQ(const ExprPtrT& expr) const
{
   return true;
};

void ExprPattern::protectScopePattern(ExprScopeT* scope) const
{
   scope->define_local(symbolname(), arg[0]);
};

void ExprPattern::defineScopePattern(ExprScopeT* scope, const ExprPtrT& expr) const
{
   EXPR_DEBUG("ExprPattern::defineScopePattern", print() + " " + expr->print())
   scope->define_local(symbolname(), expr);
};

ExprNameT ExprPattern::symbolname() const
{
   return arg[0]->symbolname();
};


std::string ExprPattern::print() const
{
   return symbolname() + "_";
};



EXPR_NAME_IMPL(ExprNoMatch, "NoMatch")

ExprNoMatch::ExprNoMatch() : Expression() {};

ExprPtrT ExprNoMatch::evaluate(ExprScopeT* scope)
{
   EXPR_EVAL_ASSERT(false, EvaluationOnNoMatch, this);
};

std::string ExprNoMatch::print() const
{
   return name();
};




// Global Constants

ExprPtrT ExprGlobalNullPtr(new ExprNull());

ExprPtrT ExprNullPtr()
{
//   EXPR_DEBUG("ExprNullPtr()", ExprGlobalNullPtr->refs)
   return ExprGlobalNullPtr;
};

ExprPtrT ExprGlobalNoPatternPtr(new ExprNoPattern());

ExprPtrT ExprNoPatternPtr()
{
//   EXPR_DEBUG("ExprNoPatternPtr()", ExprGlobalNoPatternPtr->refs)
   return ExprGlobalNoPatternPtr;
};

ExprPtrT ExprGlobalNoMatchPtr(new ExprNoMatch());

ExprPtrT ExprNoMatchPtr()
{
//   EXPR_DEBUG("ExprNoPatternPtr()", ExprGlobalNoPatternPtr->refs)
   return ExprGlobalNoMatchPtr;
};



/****************************************************************************

      Expression Pointer

*****************************************************************************/

// Only for Initialization of global null pointer
ExpressionPtr::ExpressionPtr() {
   EXPR_PTR_DEBUG("ExprPtr Begin: Default Constructor", "")
   ptr = new ExprNull();
   ptr->refs++;
   EXPR_PTR_DEBUG("ExprPtr End: Default Constructor name = Null, refs:", ptr->refs)

//   std::cout << "ExpressionPtr() called !!!" << std::endl;
//   std::cout.flush();
};

ExpressionPtr::ExpressionPtr(Expression* e) : ptr(e)
{
   EXPR_PTR_DEBUG("ExprPtr Begin: Constructor from Expression* :", e)
   if (e==0) ptr = ExprGlobalNullPtr.get();
   ptr->refs++;
   EXPR_PTR_DEBUG("ExprPtr End: Constructor from Expression*: " + (ptr->print()), ptr->refs)
};

ExpressionPtr::~ExpressionPtr()
{
   EXPR_PTR_DEBUG("ExprPtr Destructor: " + (ptr->print()), ptr->refs)
   if (--(ptr->refs) == 0) delete ptr;
};

ExpressionPtr::ExpressionPtr(const ExpressionPtr& r) : ptr(r.ptr)
{
   EXPR_PTR_DEBUG("ExprPtr Begin Copy Constructor for: " + (ptr->print()) + " refs=", ptr->refs)
   ptr->refs++;
};


Expression* ExpressionPtr::get()
{
   return ptr;
};

Expression* ExpressionPtr::get() const
{
   return ptr;
};

Expression* ExpressionPtr::operator -> ()
{
   return ptr;
};

Expression* ExpressionPtr::operator -> () const
{
   return ptr;
};

Expression& ExpressionPtr::operator * ()
{
   return *ptr;
};

Expression& ExpressionPtr::operator * () const
{
   return *ptr;
};

ExpressionPtr& ExpressionPtr::operator = (const ExpressionPtr& rhs)
{
   EXPR_PTR_DEBUG("ExprPtr operator = ExprPtr for: " + (ptr->print()) + " refs=", ptr->refs)
   EXPR_PTR_DEBUG("ExprPtr operator = ExprPtr rhs =" + (rhs->print()) + " refs=", rhs->refs)

   if (ptr != rhs.ptr)
   {
      rhs.ptr->refs++;
      if (--(ptr->refs) == 0) delete ptr;
      ptr = rhs.ptr;
   };
   return *this;
};

ExpressionPtr& ExpressionPtr::operator = (Expression* e)
{
   EXPR_PTR_DEBUG("ExprPtr operator = Expr* for: " + (ptr->print()) + " refs=", ptr->refs)
   EXPR_PTR_DEBUG("ExprPtr operator = Expr* rhs =" + (e->print()) + " refs=", e->refs)

   if (ptr != e)
   {
      e->refs++;
      if (--(ptr->refs) == 0) delete ptr;
      ptr = e;
   };
   return *this;
};

// equality of pointers !
bool ExpressionPtr::operator == (Expression* rhs) const
{
   return ptr == rhs;
};


bool ExpressionPtr::operator == (const ExpressionPtr& rhs) const
{
   return ptr == rhs.ptr;
};

bool ExpressionPtr::operator != (const ExpressionPtr& rhs) const
{
   return ptr != rhs.ptr;
};

// Pattern ordering for definitions in ExpressionScope
bool ExpressionPtr::operator < (const ExpressionPtr& rhs) const
{
   return ptr->lessPatternQ(rhs);
};



/*********************************************************************************

   ExpressionScope 

**********************************************************************************/

ExpressionScope::ExpressionScope() : slevel(0)
{
   // global ExpressionScope:
   defs.push_front(DefsT());
   top_defs = defs.begin();
   global_defs = defs.begin();

   // may initialize some global settings here
   // define("$Path", ExprPtrT(new ExprStrg("/home/ckirst/")));
};

void ExpressionScope::define(const ExprNameT& name, const ExprPtrT& e)
{
   define(name, ExprNoPatternPtr(), e);
};

void ExpressionScope::define(const ExprNameT& name, const ExprPtrT& pattern, const ExprPtrT& expr)
{
   EXPR_DEBUG("Scope:", "")
   EXPR_DEBUG("Scope: define: name=", name)
   EXPR_DEBUG("Scope: define: pattern =", pattern)
   EXPR_DEBUG("Scope: define: expr =", expr)

   // look up name
   DefsStackIter sit = top_defs;
   DefsIter      dit;

   while (sit != defs.end())
   {
      // varaible already defined
      dit = sit->find(name);
      if (dit != sit->end())
      {
         // if symbol definition overwrite all previous function defs
         if (pattern == ExprNoPatternPtr())
         {
            EXPR_DEBUG("Scope: define new symbol", "")
            (dit->second).clear(); // all function definitions are overwritten
         };

         // pattern already existent ? -> then replace expr
         PatternDefsIter pit = (dit->second).find(pattern);
         if (pit != (dit->second).end())
         {
            EXPR_DEBUG("Scope: pattern for symbol already defined", "")
            pit->second = expr;
            return;
         }

         // make definition definiton 
         EXPR_DEBUG("Scope: define insert on local scope?: ", (sit != global_defs))
         (dit->second).insert(std::pair<ExprPtrT, ExprPtrT>(pattern, expr));
         return;
      };
      sit++;
   };

   // not defined previously -> new global definition
   EXPR_DEBUG("Scope: define: insert on global scope", "")
   PatternDefsT p;
   p.insert(std::pair<ExprPtrT, ExprPtrT>(pattern, expr));
   global_defs->insert(std::pair<ExprNameT, PatternDefsT>(name, p));
};

void ExpressionScope::define_local(const ExprNameT& name, const ExprPtrT& expr)
{
   define_local(name, ExprNoPatternPtr(), expr);
};

void ExpressionScope::define_local(const ExprNameT& name, const ExprPtrT& pattern, const ExprPtrT& expr)
{
   EXPR_DEBUG("Scope: define_local: name=", name)
   EXPR_DEBUG("Scope: define_local: pattern =", pattern)
   EXPR_DEBUG("Scope: define_local: expr =", expr)

   // look up name
   DefsStackIter sit = top_defs;
   DefsIter      dit = sit->find(name);
   // varaible locally defined ?
   if (dit != sit->end())
   {
      // new symbol definition 
      if (pattern == ExprNoPatternPtr())
      {
         EXPR_DEBUG("Scope: define_local: new symbol", "")
         (dit->second).clear(); // all function definitions are overwritten
      };

      // definiton 
      EXPR_DEBUG("Scope: define_local: insert pattern: ", (sit != global_defs))
      (dit->second).insert(std::pair<ExprPtrT, ExprPtrT>(pattern, expr));
      return;
   };

   // not defined previously -> local definition
   EXPR_DEBUG("Scope: define_local: insert on local scope", "")
   PatternDefsT p;
   p.insert(std::pair<ExprPtrT, ExprPtrT>(pattern, expr));
   top_defs->insert(std::pair<ExprNameT, PatternDefsT>(name, p));
};

ExprPtrT ExpressionScope::match(const ExprNameT& name) const
{
   return (match(name, ExprNoPatternPtr())).second;
};

std::pair<ExprPtrT,ExprPtrT> ExpressionScope::match(const ExprNameT& name, const ExprPtrT& expr) const
{
   EXPR_DEBUG("Scope: match: name=", name)
   EXPR_DEBUG("Scope: match: expr=", expr)

   // look up name
   DefsStackIter sit = top_defs;
   DefsIter dit; 

   while (sit != defs.end())
   {
      dit = sit->find(name);
      if (dit != sit->end())
      {
         // match one of the patterns
         PatternDefsIter pit = (dit->second).begin();
         while (pit != (dit->second).end())
         {
            if (pit->first->matchQ(expr)) 
            {
               EXPR_DEBUG("Scope: match is:", "")
               EXPR_DEBUG( pit->first, pit->second)
               return (*pit);
            };
            pit++;
         };
         EXPR_DEBUG("Scope: symbol found but no matching pattern", "")
         return std::pair<ExprPtrT, ExprPtrT>(ExprNoPatternPtr(), ExprNoMatchPtr());
      };
      sit++;
   };
   EXPR_DEBUG("Scope: no match found", "")
   return std::pair<ExprPtrT, ExprPtrT>(ExprNoPatternPtr(), ExprNoMatchPtr());
};

void ExpressionScope::pop()
{
   EXPR_DEBUG("Scope: pop() slevel=", slevel)
   EXPR_EVAL_ASSERT(slevel>0, ScopeUnderflow, ExprNullPtr())
   defs.pop_front();
   top_defs = defs.begin();
   slevel--;
};

void ExpressionScope::push()
{
   EXPR_DEBUG("Scope: push() slevel=", slevel)
   EXPR_EVAL_ASSERT(slevel < EXPR_MAX_SCOPELEVEL, MaxScope, ExprNullPtr())
   defs.push_front(DefsT());
   top_defs = defs.begin();
   slevel++;
};


ExpressionScope::ScopeLevelT ExpressionScope::level() const
{
   return slevel;
};

void ExpressionScope::clear()
{
   while (defs.begin() != global_defs) pop();
   global_defs->clear();
   top_defs = global_defs;
};

bool ExpressionScope::defined(const ExprNameT& name)
{
   return defined(name, ExprNoPatternPtr());
};

bool ExpressionScope::defined(const ExprNameT& name, const ExprPtrT& pattern)
{
   std::pair<ExprPtrT, ExprPtrT> res = match(name, pattern);
   if (res.second == ExprNoMatchPtr()) return false;
   else return true;
};





std::string ExpressionScope::info()
{
   EXPR_DEBUG("scope.info()", "")

   std::ostringstream str;

   ScopeLevelT sl = slevel;
   for (DefsStackIter it = defs.begin(); it != global_defs; it++)
   {
      EXPR_DEBUG("scope.info()", sl)

      str << "Definitions on Scope level: " << sl << '\n';
      str << print_definitions(it);
      str << '\n';
   };
   str << "Global Definitions:" << std::endl;
   str << print_definitions(global_defs);
   return str.str();
};

std::string ExpressionScope::print_definitions(DefsStackIter ds)
{
   std::ostringstream str;
   for (DefsIter dit = ds->begin(); dit != ds->end(); dit++)
   {
      for (PatternDefsIter pit = dit->second.begin(); pit != dit->second.end(); pit++)
      {
         str << dit->first;
         if (pit->first != ExprNoPatternPtr())
         {
            str << "[";
            for (ExprArgIteratorT eit = (pit->first)->arg.begin(); eit != (pit->first)->arg.end(); eit++)
            {
               str << *eit;
               if (eit != ((pit->first)->arg.end() - 1)) str << ",";
            };
            str << "]";
         };
         str << " = " << pit->second << '\n';
      };
   };
   return str.str();
};



/*********************************************************************************

   io

**********************************************************************************/

std::ostream& operator<<(std::ostream& os, Expression* e)
{
   return os << e->print();
};

std::ostream& operator<<(std::ostream& os, const ExprPtrT& e)
{
   return os << e->print();
};

/*
std::ostream& operator>>(std::ostream& os, const ExprPtrT& e)
{
   return os >> .... 
   parse e->print();
};
*/






/*********************************************************************************

   Evaluation Error Descriptions 

**********************************************************************************/

ExprEvalErrorDescriptionT ExprEvalErrorDescriptionO;

void InitEvalErrorDescription()
{
   ExprEvalErrorDescriptionO[NoEvalError] 
   = "No Error!";

   ExprEvalErrorDescriptionO[NonNumberToReal]
   = "Cannot convert Expression to Real!";
   ExprEvalErrorDescriptionO[NonNumberToIntg]
   = "Cannot convert Expression to Intg!";
   ExprEvalErrorDescriptionO[NonStrgToStrg]
   = "Cannot convert Expression to Strg!";
   ExprEvalErrorDescriptionO[NonBoolToBool]
   = "Cannot convert Expression to Bool!";

   ExprEvalErrorDescriptionO[MaxRecursion]
   = "Max recursion limit reached!";
   ExprEvalErrorDescriptionO[MaxScope]
   = "Max scope limit reached!";

   ExprEvalErrorDescriptionO[EvaluateAtOnAtom]
   = "EvaluateAt ([] operator) on Atom!";
   ExprEvalErrorDescriptionO[EvaluateAtIllegalArgs]
   = "EvaluateAt ([] operator): Argument number does not match function definition!";

   ExprEvalErrorDescriptionO[ExtractOutOfRange]
   = "Extract index out of List range!";
   ExprEvalErrorDescriptionO[InsertOutOfRange]
   = "Insert index out of List range!";
   ExprEvalErrorDescriptionO[ReplaceOutOfRange]
   = "Replace index out of List range!";

   ExprEvalErrorDescriptionO[ReturnToGlobal]
   = "Return in global scope!";
   ExprEvalErrorDescriptionO[BreakToGlobal]
   = "Break in global scope!";

   ExprEvalErrorDescriptionO[DivisionByZero]
   = "Division by Zero!";

   ExprEvalErrorDescriptionO[IteratorExpectList] 
   = "Iterator: Single argument Expected to evaluate to List!";
   ExprEvalErrorDescriptionO[IteratorExpectNumber] 
   = "Iterator: Second to fourth arguments in iterator list Expected to evaluate to Numbers!";
   ExprEvalErrorDescriptionO[IteratorLoop] 
   = "Iterator: Interdependent Iterators!";

   ExprEvalErrorDescriptionO[CreatorExpectList] 
   = "Creator: Single argument Expected to evaluate to List!";
   ExprEvalErrorDescriptionO[CreatorExpectNumber] 
   = "Creator: Second to fourth arguments in iterator list Expected to evaluate to Numbers!";

   ExprEvalErrorDescriptionO[RandomToNumber]
   = "Cannot convert random to Number as arguments are non-numbers!";
   ExprEvalErrorDescriptionO[RandomSeedNotInteger]
   = "Seed Expects Integer!";

   ExprEvalErrorDescriptionO[TypeMismatch] 
   = "Data types do not match!";

   ExprEvalErrorDescriptionO[EvaluateBase] 
   = "Internal Error: EvaluateBase!";
   ExprEvalErrorDescriptionO[RecursionUnderflow] 
   = "Internal Error: RecursionUnderflow!";
   ExprEvalErrorDescriptionO[ScopeUnderflow] 
   = "Internal Error: ScopeUnderflow!";
   ExprEvalErrorDescriptionO[OutOfArgRange] 
   = "Internal Error: OutOfArgRange!";
   ExprEvalErrorDescriptionO[SymbolNameOnNonSymbol] 
   = "Internal Error: SymbolNameOnNonSymbol!";
   ExprEvalErrorDescriptionO[EvaluationOnNoMatch] 
   = "Internal Error: EvaluationOnNoMatch!";

   ExprEvalErrorDescriptionO[IteratorInternalError]
   = "Internal Error: IteratorInternalError!";
};


std::string ExprEvalErrorDescription(const ExprEvalErrorT& e) 
{
   InitEvalErrorDescription();
   return ExprEvalErrorDescriptionO[e];
};

/*********************************************************************************

   Syntax Error Descriptions 

**********************************************************************************/
ExprSyntaxErrorDescriptionT ExprSyntaxErrorDescriptionO;

void InitSyntaxErrorDescription()
{
   ExprSyntaxErrorDescriptionO[NoSyntaxError]
   = "No Syntax Error!";
   ExprSyntaxErrorDescriptionO[SyntaxError]
   = "General Syntax Error!";

   ExprSyntaxErrorDescriptionO[IllegalArgumentNumber]
   = "Illegal argument number!";

   ExprSyntaxErrorDescriptionO[PatternExpectSymbol]
   = "Pattern Expects Symbol as argument!";

   ExprSyntaxErrorDescriptionO[SetExpectSymbol]
   = "Set (=) Expects Symbol before [] operator on lhs!";
   ExprSyntaxErrorDescriptionO[SetExpectFunctionOrSymbol] 
   = "Set (=) Expects Symbol or EvaluateAt expression on lhs!";

   ExprSyntaxErrorDescriptionO[DefineExpectSymbol]
   = "Define (:=) Expects Symbol before [] operator on lhs!";
   ExprSyntaxErrorDescriptionO[DefineExpectFunctionOrSymbol] 
   = "Define (:=) Expects Symbol or EvaluateAt expression on lhs!";

   ExprSyntaxErrorDescriptionO[TableExpectList] 
   = "Second Arument of Table has to be List of four elements!";
   ExprSyntaxErrorDescriptionO[TableExpectSymbol] 
   = "Table Expects Symbol in the first position of iterator List!";

   ExprSyntaxErrorDescriptionO[FunctionExpectSymbolList] 
   = "Function Expects List of Symbols as fist argument!";
   ExprSyntaxErrorDescriptionO[ModuleExpectSymbolList] 
   = "Module Expects List of Symbols as fist argument!";
   ExprSyntaxErrorDescriptionO[ModuleExpectBlock] 
   = "Module Expects Block as second argument!";

   ExprSyntaxErrorDescriptionO[IteratorExpectSymbol] 
   = "Iterator Expects Symbol in the first position of iterator List!";
   ExprSyntaxErrorDescriptionO[IteratorExpectIterationList] 
   = "Iterator Expects List of three or four expressions as second argument";
   ExprSyntaxErrorDescriptionO[IteratorSyntaxError] 
   = "Iterator syntax error: Expected Iterator[expr, {iter, start, end (,delta)}] or Iterator[expr]!";

   ExprSyntaxErrorDescriptionO[CreatorExpectSymbol] 
   = "Creator Expects Symbol in the first position of iterator List!";
   ExprSyntaxErrorDescriptionO[CreatorExpectIterationList] 
   = "Creator Expects List of three or four expressions as second argument";
   ExprSyntaxErrorDescriptionO[CreatorSyntaxError] 
   = "Creator syntax error: Expected Creator[expr, {iter, start, end (,delta)}] or Creator[expr]!";
};

std::string ExprSyntaxErrorDescription(const ExprSyntaxErrorT& e)
{
   InitSyntaxErrorDescription();
   return ExprSyntaxErrorDescriptionO[e];
};
