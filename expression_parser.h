/**************************************************************************
   expression_parser.h  -  grammar to parse input to a list of expressions

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
***************************************************************************/
#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

// Boost Includes:
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/error_handling/exceptions.hpp>

// Standard Includes:
#include <string>                             // string, getline
#include <iostream>                           // cout, cerr
#include <sstream>                            // istringstream, ostringstream
#include <iomanip>                            // setw, setfill
#include <map>                                // map
#include <stack>
#include <fstream>

// Expressions
#include "expression.h"
#include "expression_basic.h"
#include "expression_sim.h"
#include "expression_extra.h"


using namespace std;
using namespace boost::spirit;
//using namespace boost;

typedef ExprNameT NameT;

#include <assert.h>
#define PARSER_ASSERT(cond, str)  if (!(cond)) { std::cout << "ExprParser error: " << str << std::endl; assert(cond); };

//#define PARSER_DEBUG(a1, a2) std::cout << a1 << " " << a2 << endl; std::cout.flush();
#define PARSER_DEBUG(a1, a2)  

template<typename ET>
ExprPtrT createFunction(const char* str, const ExprPtrT& e) 
{
   ExprPtrT r(new ET());
   r->arg = e->arg;
   return r;
};

template<typename ET>
ExprPtrT createSymbol(const char* str) 
{
   ExprPtrT r(new ET());
   return r;
};

class ExprParser
{
public:
   ExprParser();
   ~ExprParser() {};

   bool parse_file(const char* filename, std::string& errmsg);
   bool parse(const std::string& input, std::string& errmsg);
   bool parse(const char* input, const char* last, std::string& errmsg);

   ExprPtrT  result;

private:

   // Errors to check for during the parse:
   enum ParseError {
                     NoParserError = 0,

                     ExpectExpression,
                     ExpectLeftBracket,
                     ExpectRightBrace,
                     ExpectRightBracket,
                     ExpectRightParen,
                     ExpectRightRef,
                     ExpectRightUnderline,
                     ExpectRightString,
                     ExpectAssign,
                     ExpectDef,
                     ExpectEnd
                  };

   // Comments
   struct CommentGrammar : public grammar<CommentGrammar>
   {
      template <typename ScannerT>
      struct definition
      {
        definition(CommentGrammar const& self)
        {
            skip_until_eol =  *(('\\' >> eol_p) | (anychar_p - eol_p));

            comment =   (  space_p
                        |  comment_p("/*", "*/")  
                        |  "//" >> skip_until_eol
                        |  '#'  >> skip_until_eol
                        );
        }

        rule<ScannerT>  skip_until_eol, comment;

        rule<ScannerT> const&
        start() const { return comment; }
      }; 
   };

   // Assertions to use during the parse:
   static assertion<ParseError> Expect_expression;
   static assertion<ParseError> Expect_left_bracket;
   static assertion<ParseError> Expect_right_bracket;
   static assertion<ParseError> Expect_right_brace;
   static assertion<ParseError> Expect_right_paren;
   static assertion<ParseError> Expect_right_ref;
   static assertion<ParseError> Expect_right_underline;
   static assertion<ParseError> Expect_right_string;
   static assertion<ParseError> Expect_end;

   typedef std::stack<ExprPtrT> ParserExprStackT;
   ParserExprStackT expr_stack;

   template<typename ET>
   struct CreateAtom
   {
      CreateAtom(ParserExprStackT& stack_) : stack(stack_) {};

      void operator()(char const* first, char const* last) const
      {
         PARSER_DEBUG("CreateAtom", std::string(first, last))
         stack.push(ExprPtrT(new ET(std::string(first, last))));
      };

      ParserExprStackT& stack;
   };

   template<typename EU>
   struct CreateUnary
   {
      CreateUnary(ParserExprStackT& stack_) : stack(stack_) {};

      void operator()(char const* str, char const* end) const
      {
         PARSER_DEBUG("CreateUnary ()(const char*, const char*)", std::string(str, end))
         ExprPtrT e = stack.top();
         stack.pop();
         stack.push(ExprPtrT(new EU(e)));
         ExprSyntaxErrorT err = stack.top()->check_syntax();
         if (err != NoSyntaxError) throw_<ExprSyntaxErrorT, const char*>(str, err);
      };

      ParserExprStackT& stack;
   };

   template<typename EB>
   struct CreateBinary
   {
      CreateBinary(ParserExprStackT& stack_) : stack(stack_) {};

      void operator()(char const* str, char const* end) const
      {
         PARSER_DEBUG("CreateBinary", std::string(str, end))
         ExprPtrT er = stack.top();
         stack.pop();
         ExprPtrT el = stack.top();
         stack.pop();
         stack.push(ExprPtrT(new EB(el, er)));
         ExprSyntaxErrorT e = stack.top()->check_syntax();
         if (e != NoSyntaxError) throw_<ExprSyntaxErrorT, const char*>(str, e);
      };

      ParserExprStackT& stack;
   };

   typedef std::map< ExprNameT, ExprPtrT (*)(const char*, const ExprPtrT&) >  ParserPredefinedFuncT;
   typedef ParserPredefinedFuncT::value_type ParserPredefinedFuncValueT;
   ParserPredefinedFuncT predefined_functions;

   template< typename ET >
   void predefineFunction(ParserPredefinedFuncT& pf)
   {
      pf.insert(ParserPredefinedFuncValueT(ET::Name , &createFunction<ET>));
   };

   struct CreateEvaluation
   {
      CreateEvaluation(ParserExprStackT& stack_, ParserPredefinedFuncT& pf_) : stack(stack_), predef(pf_) {};

      void operator()(char const* str, char const* end) const
      {
         PARSER_DEBUG("CreateEvaluation", std::string(str, end))

         ExprPtrT er = stack.top();
         stack.pop();
         ExprPtrT el = stack.top();
         stack.pop();

         if (el->symbolQ())
         {
            ParserPredefinedFuncT::iterator pre = predef.find(el->symbolname());
            if (pre != predef.end())
            {
               stack.push((pre->second)(str, er));
            } else {
               stack.push(ExprPtrT(new ExprEvaluateAt(el, er)));
            };
         } else {
           stack.push(ExprPtrT(new ExprEvaluateAt(el, er)));
         };

         ExprSyntaxErrorT e = stack.top()->check_syntax();
         if (e != NoSyntaxError) throw_<ExprSyntaxErrorT, const char*>(str, e);
      };

      ParserExprStackT& stack;
      ParserPredefinedFuncT& predef;
   };

   typedef std::map<ExprNameT, ExprPtrT (*)(const char*) >   ParserPredefinedSymbT;
   typedef ParserPredefinedSymbT::value_type      ParserPredefinedSymbValueT;
   ParserPredefinedSymbT predefined_symbols;

   template<typename ET>
   void predefineSymbol(ParserPredefinedSymbT& ps)
   {
      ps.insert(ParserPredefinedSymbValueT(ET::Name , &createSymbol<ET>));
   };

   struct CreateSymbol
   {
      CreateSymbol(ParserExprStackT& stack_, ParserPredefinedSymbT& ps_) : stack(stack_), predef(ps_) {};

      void operator()(char const* str, char const* end) const
      {
         PARSER_DEBUG("CreateSymbol", std::string(str, end))

         ParserPredefinedSymbT::iterator pre = predef.find(std::string(str,end));
         if (pre != predef.end())
         {
            stack.push((pre->second)(str));
         } 
         else 
         {
            stack.push(ExprPtrT(new ExprSymbol(std::string(str, end))));
         };
      };

      ParserExprStackT& stack;
      ParserPredefinedSymbT& predef;
   };


   enum ParserScopeTypeT {List = 0, Func, Decl, Extr, Stmt};

   struct ParserScopeT
   {
      typedef std::stack<ExprIndexT> ParserScopeArgsStackT;
      typedef std::stack<ParserScopeTypeT> ParserScopeTypeStackT;
      ParserScopeArgsStackT args;
      ParserScopeTypeStackT type;
   };

   ParserScopeT scope_stack;

   template<ParserScopeTypeT st>
   struct NewScope
   {
      NewScope(ParserScopeT& sc) : scope(sc) {};

      void operator()(char const* str, char const* end) const
      {
         PARSER_DEBUG("NewScope", std::string(str, end))
         scope.type.push(st);
         scope.args.push(0);
      };

      ParserScopeT& scope;
   };

   template<ParserScopeTypeT st>
   struct IncreaseArg
   {
      IncreaseArg(ParserScopeT& sc) : scope(sc) {};

      void operator()(char const* str, char const* end) const
      {
         PARSER_DEBUG("InceaseArg", std::string(str, end))
         PARSER_ASSERT(st == scope.type.top(), "IncreaseArg: Scope type Mismatch!")
         scope.args.top() = scope.args.top() + 1;
      };

      ParserScopeT& scope;
   };

   template<ParserScopeTypeT st, typename ET>
   struct CreateList
   {
      CreateList(ParserExprStackT& sta, ParserScopeT& sc) 
      : stack(sta), scope(sc) {};

      void operator()(char const* str, char const* end) const
      {
         PARSER_DEBUG("CreateList", std::string(str, end))
         PARSER_ASSERT(st == scope.type.top(), "CreateFromScope: Scope type Mismatch!")
         ExprPtrT e(new ET());
         while (scope.args.top() > 0)
         {
            PARSER_ASSERT(stack.size() > 0, "CreateFromScope: Expr Stack empty!")
            e->prepend(stack.top());
            stack.pop();
            scope.args.top() = scope.args.top()-1;
         };
         stack.push(e);
         scope.args.pop();
         scope.type.pop();

         ExprSyntaxErrorT err = stack.top()->check_syntax();
         if (err != NoSyntaxError) throw_<ExprSyntaxErrorT, const char*>(str, err);
      };

      ParserExprStackT& stack;
      ParserScopeT& scope;
   };

   // Grammar
   struct SimGrammar : public grammar<SimGrammar> 
   {
   SimGrammar(ParserExprStackT& stack_, ParserScopeT& scope_, ParserPredefinedFuncT& pf_, ParserPredefinedSymbT& ps_)
   : stack(stack_), scope(scope_), predef_func(pf_), predef_symb(ps_)
   {};

   ParserExprStackT& stack;
   ParserScopeT& scope;
   ParserPredefinedFuncT& predef_func;
   ParserPredefinedSymbT& predef_symb;

   template<typename ScannerT> 
   struct definition {
      rule<ScannerT> integer, real, strng, boolean, literal;
      rule<ScannerT> list, evaluation, extract, parenthesis, pattern;
      rule<ScannerT> base, at, power, factor, multiplication, addition;
      rule<ScannerT> comparison, and_expression, or_expression, expression;
      rule<ScannerT> block, global;
      rule<ScannerT> run;

      definition(SimGrammar const& self)
      {
         integer  =  (lexeme_d[  +digit_p  ])[CreateAtom<ExprInteger>(self.stack)];

         real = lexeme_d[ ( ch_p('.') 
                            >> +digit_p 
                            >> !(  (ch_p('e') | ch_p('E'))
                                    >> !(ch_p('+') | ch_p('-')) >> +digit_p
                                )
                          | +digit_p 
                          >> (  (ch_p('.') >> *digit_p)
                             || ( (ch_p('e') | ch_p('E'))
                                  >> !(ch_p('+') | ch_p('-')) >> +digit_p
                                )
                             )
                          ) 
                        ][CreateAtom<ExprReal>(self.stack)];

         strng = ( ch_p('"') 
                   >> *(lexeme_d[ c_escape_ch_p ] - ch_p('"')) 
                   >> Expect_right_string( ch_p('"') ) 
                 )[CreateAtom<ExprString>(self.stack)];

         boolean = ( str_p("True") | str_p("False") )[CreateAtom<ExprBool>(self.stack)];

         literal = lexeme_d[ ((alpha_p | '$') >> *(alnum_p | '$' | '.')) ][CreateSymbol(self.stack, self.predef_symb)];

         list = ( str_p("{")[NewScope<List>(self.scope)]
                  >> !(expression[IncreaseArg<List>(self.scope)]
                       >> *( (ch_p(',') 
                              >> Expect_expression(expression))[IncreaseArg<List>(self.scope)] )
                      )
                  >> Expect_right_brace( ch_p('}') )
                )[CreateList<List, ExprList>(self.stack, self.scope)];

         evaluation = ( (str_p("[")-str_p("[["))[NewScope<Func>(self.scope)]
                        >> !(expression[IncreaseArg<Func>(self.scope)]
                             >> *( (ch_p(',')
                                    >> Expect_expression(block))[IncreaseArg<Func>(self.scope)] 
                                 )
                            )
                        >> Expect_right_bracket( ch_p(']') )
                      )[CreateList<Func, ExprList>(self.stack, self.scope)]
                       [CreateEvaluation(self.stack, self.predef_func)];

         extract = ( str_p("[[")[NewScope<Extr>(self.scope)]
                     >> Expect_expression(expression)[IncreaseArg<Extr>(self.scope)]
                     >> *((ch_p(',') 
                           >> Expect_expression(expression))[IncreaseArg<Extr>(self.scope)] 
                         )
                     >> Expect_right_ref( str_p("]]") )
                   )[CreateList<Extr, ExprSequence>(self.stack, self.scope)]
                    [CreateBinary<ExprExtract>(self.stack)];

         parenthesis = ( ch_p('(')
                         >> Expect_expression(expression)
                         >> Expect_right_paren( ch_p(')') )
                       );

         pattern = str_p("_")[CreateUnary<ExprPattern>(self.stack)];

         base = (  parenthesis
                |  list
                |  real
                |  integer
                |  strng
                |  boolean
                |  literal >> !pattern 
                ) >> *(evaluation) >> *(extract);

         at = base >> 
                  *( ( str_p("@")[NewScope<Func>(self.scope)]
                       >> Expect_expression(at)[IncreaseArg<Func>(self.scope)]
                                                 [CreateList<Func, ExprList>(self.stack, self.scope)]
                                                 [CreateEvaluation(self.stack, self.predef_func)]
                     )
                   );

         power = at >> 
                 *( ( ch_p('^') 
                      >> Expect_expression(factor)
                    )[CreateBinary<ExprPower>(self.stack)] 
                  );

         factor = ( (ch_p('!') >> factor)[CreateUnary<ExprNot>(self.stack)]
                  | (ch_p('-') >> factor)[CreateUnary<ExprMinus>(self.stack)]
                  | (ch_p('+') >> factor)
                  | power
                  );

         multiplication = factor >> 
            *( (ch_p('*') >> Expect_expression(factor))[CreateBinary<ExprMultiply>(self.stack)]
             | (ch_p('/') >> Expect_expression(factor))[CreateBinary<ExprDivide>(self.stack)] 
             | (ch_p('%') >> Expect_expression(factor))[CreateBinary<ExprMod>(self.stack)] 
             );

         addition = multiplication >> 
            *( (ch_p('+') >> Expect_expression(multiplication))[CreateBinary<ExprPlus>(self.stack)]
             | (ch_p('-')
                >> Expect_expression(multiplication))[CreateBinary<ExprSubtract>(self.stack)] 
             );

         comparison = addition >> 
            *( (str_p("==") >> Expect_expression(addition))[CreateBinary<ExprEqual>(self.stack)]
             | (str_p("!=") >> Expect_expression(addition))[CreateBinary<ExprNotEqual>(self.stack)] 
             | (str_p("<=") >> Expect_expression(addition))[CreateBinary<ExprLessEqual>(self.stack)]
             | (str_p(">=") 
                >> Expect_expression(addition))[CreateBinary<ExprGreaterEqual>(self.stack)]
             | (str_p("<")  >> Expect_expression(addition))[CreateBinary<ExprLess>(self.stack)]
             | (str_p(">")  >> Expect_expression(addition))[CreateBinary<ExprGreater>(self.stack)]
             );

         and_expression = comparison >> 
            *( (str_p("&&") >> Expect_expression(comparison))[CreateBinary<ExprAnd>(self.stack)] );

         or_expression = and_expression >>
            *( (str_p("||") >> Expect_expression(and_expression))[CreateBinary<ExprOr>(self.stack)]
             );

         expression = or_expression >>
            *( (ch_p('=')  >> Expect_expression( or_expression ))[CreateBinary<ExprSet>(self.stack)] 
             | (str_p(":=") 
                >> Expect_expression( or_expression ))[CreateBinary<ExprDefine>(self.stack)]
             );

         block = expression >> 
             *( str_p(";")[NewScope<Stmt>(self.scope)][IncreaseArg<Stmt>(self.scope)]
                >> *( (expression >> Expect_end( ch_p(';') ))[IncreaseArg<Stmt>(self.scope)] )
                >> eps_p[CreateList<Stmt, ExprBlock>(self.stack, self.scope)]
              );

         global = ( eps_p[NewScope<Stmt>(self.scope)] >>
                    *( (expression >> Expect_end( ch_p(';') ))[IncreaseArg<Stmt>(self.scope)] )
                    >> eps_p[CreateList<Stmt, ExprGlobal>(self.stack, self.scope)]
                  );

         run = global;
      }

      rule<ScannerT> const&
      start() const { return run; }
   };

   }; // SimGrammar

private:

   std::string print_error(char const* in, char const* pos);

}; // class parser

#endif
