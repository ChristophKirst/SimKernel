/***************************************************************************
   expression_basic.cpp   -  basic code expressions 
   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#include "expression_basic.h"

EXPR_NAME_IMPL(ExprReal,         "Real")
EXPR_NAME_IMPL(ExprInteger,      "Integer")
EXPR_NAME_IMPL(ExprString,       "String")
EXPR_NAME_IMPL(ExprBool,         "Bool")
EXPR_NAME_IMPL(ExprSymbol,       "Symbol")

EXPR_NAME_IMPL(ExprPi,           "Pi")
EXPR_NAME_IMPL(ExprE,            "E")
EXPR_NAME_IMPL(ExprGoldenRatio,  "GoldenRatio")

EXPR_NAME_IMPL(ExprAll,          "All")
EXPR_NAME_IMPL(ExprAutomatic,    "Automatic")
EXPR_NAME_IMPL(ExprNone,         "None")

EXPR_NAME_IMPL(ExprSequence,     "Sequence")
EXPR_NAME_IMPL(ExprList,         "List")
EXPR_NAME_IMPL(ExprTable,        "Table")
EXPR_NAME_IMPL(ExprExtract,      "Extract")
EXPR_NAME_IMPL(ExprAppend,       "Append")
EXPR_NAME_IMPL(ExprPrepend,      "Prepend")
EXPR_NAME_IMPL(ExprReplace,      "Replace")
EXPR_NAME_IMPL(ExprInsert,       "Insert")
EXPR_NAME_IMPL(ExprJoin,         "Join")
EXPR_NAME_IMPL(ExprLength,       "Length")

EXPR_NAME_IMPL(ExprFunction,     "Function")
EXPR_NAME_IMPL(ExprEvaluateAt,   "EvaluateAt")

EXPR_NAME_IMPL(ExprSet,          "Set")
EXPR_NAME_IMPL(ExprDefine,       "Define")


EXPR_NAME_IMPL(ExprBlock,        "Block")
EXPR_NAME_IMPL(ExprGlobal,       "Global")
EXPR_NAME_IMPL(ExprReturn,       "Return")
EXPR_NAME_IMPL(ExprBreak,        "Break")
EXPR_NAME_IMPL(ExprModule,       "Module")

EXPR_NAME_IMPL(ExprIf,           "If")
EXPR_NAME_IMPL(ExprFor,          "For")

EXPR_NAME_IMPL(ExprPlus,                        "Plus")
EXPR_BIOP_IMPL(ExprSubtract,  ExprSubtractOp,   "Subtract", "-")
EXPR_BIOP_IMPL(ExprMultiply,  ExprMultiplyOp,   "Multiply", "*")
EXPR_NAME_IMPL(ExprDivide,                      "Divide")
EXPR_BIOP_IMPL(ExprMod,       ExprModOp,        "Mod",      "%")
EXPR_BIOP_IMPL(ExprPower,     ExprPowerOp,      "Power",    "^")
EXPR_NAME_IMPL(ExprMinus,                       "Minus")

EXPR_MATU_IMPL(ExprSin,  ExprSinOp,        "Sin")
EXPR_MATU_IMPL(ExprCos,  ExprCosOp,        "Cos")
EXPR_MATU_IMPL(ExprTan,  ExprTanOp,        "Tan")
EXPR_MATU_IMPL(ExprSinh, ExprSinhOp,       "Sinh")
EXPR_MATU_IMPL(ExprCosh, ExprCoshOp,       "Cosh")
EXPR_MATU_IMPL(ExprExp,  ExprExpOp,        "Exp")
EXPR_MATU_IMPL(ExprLog,  ExprLogOp,        "Log")

EXPR_NAME_IMPL(ExprNot,                               "Not")
EXPR_BIOP_IMPL(ExprAnd,          ExprAndOp,           "And",         "&&")
EXPR_BIOP_IMPL(ExprOr,           ExprOrOp,            "Or",          "||")
EXPR_BIOP_IMPL(ExprGreater,      ExprGreaterOp,       "Greater",     ">" )
EXPR_BIOP_IMPL(ExprLess,         ExprLessOp,          "Less",        "<")
EXPR_BIOP_IMPL(ExprEqual,        ExprEqualOp,         "Equal",       "==")
EXPR_BIOP_IMPL(ExprNotEqual,     ExprNotEqualOp,      "NotEqual",    "!=")
EXPR_BIOP_IMPL(ExprGreaterEqual, ExprGreaterEqualOp,  "GreaterEqual",">=")
EXPR_BIOP_IMPL(ExprLessEqual,    ExprLessEqualOp,     "LessEqual",   "<=")

EXPR_NAME_IMPL(ExprPrint,        "Print")



/***********************************************************************

    Type Conversions

************************************************************************/
ExprPtrT ValueToExpr(const int& v)
{ 
   return ExprPtrT(new ExprInteger(v)); 
};

ExprPtrT ValueToExpr(const double& v)
{ 
   return ExprPtrT(new ExprReal(v)); 
};

ExprPtrT ValueToExpr(const bool& v)
{ 
   return ExprPtrT(new ExprBool(v)); 
};

ExprPtrT ValueToExpr(const std::string& v)
{ 
   return ExprPtrT(new ExprString(v)); 
};

ExprPtrT ValueToExpr(const char* v)
{ 
   return ExprPtrT(new ExprString(v)); 
};

std::string toExprString(const char* c)
{
   return std::string("\"") + std::string(c) + "\"";
};

std::string toExprString(const std::string& c)
{
   return std::string("\"") + c + "\"";
};

std::string toExprString(const bool& val)
{
   return (val ? std::string("True") : std::string("False"));
};


template<> std::string typeName<double>() { return std::string("double"); };

template<> std::string typeName<const char*>() { return std::string("const char*"); };

template<> std::string typeName<int>() { return std::string("int"); };

template<> std::string typeName<long>() { return std::string("long"); };

template<> std::string typeName<unsigned int>() { return std::string("unsigned int"); };

template<> std::string typeName<unsigned long int>() { return std::string("unsigned long int"); };

template<> std::string typeName<short>() { return std::string("short"); };

template<> std::string typeName<short unsigned int>() { return std::string("short unsigned int"); };

template<> std::string typeName<std::string>() { return std::string("string"); };

template<> std::string typeName<bool>() { return std::string("bool"); };


void ExprToValue(const ExprPtrT& e, float& val)
{
   if (!(e->toTypeQ(typeid(double)))) 
      throw ExpressionError(ExprToValueErrorDescription<double>(e));
   val = double(*e);
};

void ExprToValue(const ExprPtrT& e, long int& val)
{
   if (!(e->toTypeQ(typeid(int)))) 
      throw ExpressionError(ExprToValueErrorDescription<int>(e));
   val = int(*e);
};

void ExprToValue(const ExprPtrT& e, unsigned long int& val)
{
   if (!(e->toTypeQ(typeid(int)))) 
      throw ExpressionError(ExprToValueErrorDescription<int>(e));
   val = int(*e);
};

void ExprToValue(const ExprPtrT& e, unsigned int& val)
{
   if (!(e->toTypeQ(typeid(int)))) 
      throw ExpressionError(ExprToValueErrorDescription<int>(e));
   val = int(*e);
};

void ExprToValue(const ExprPtrT& e, short int& val)
{
   if (!(e->toTypeQ(typeid(int)))) 
      throw ExpressionError(ExprToValueErrorDescription<int>(e));
   val = int(*e);
};

void ExprToValue(const ExprPtrT& e, short unsigned int& val)
{
   if (!(e->toTypeQ(typeid(int)))) 
      throw ExpressionError(ExprToValueErrorDescription<int>(e));
   val = int(*e);
};
