/**************************************************************************
   expression_parser.cpp - parse input text to an abstract syntax tree
                assoicated with corresponding symbolids

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation
   HU Berlin, BCCN Göttingen & Berlin (2008)
***************************************************************************/
#include "expression_parser.h"

// Boost Includes:
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/error_handling/exceptions.hpp>
//#define BOOST_SPIRIT_DEBUG

// Standard Includes:
#include <string>                             // string, getline
#include <iostream>                           // cout, cerr
#include <sstream>                            // istringstream, ostringstream
#include <iomanip>                            // setw, setfill
#include <map>                                // map
#include <vector>
#include <fstream>

using namespace std;
using namespace boost::spirit;
using namespace boost;

ExprParser::ExprParser() : result(ExprNullPtr())
{
   PARSER_DEBUG("ExprParser Constructor", "");
   expr_stack.push(ExprNullPtr());

   predefineFunction<ExprSequence>     (predefined_functions);
   predefineFunction<ExprList>         (predefined_functions);
   predefineFunction<ExprTable>        (predefined_functions);
   predefineFunction<ExprExtract>      (predefined_functions);
   predefineFunction<ExprAppend>       (predefined_functions);
   predefineFunction<ExprPrepend>      (predefined_functions);
   predefineFunction<ExprReplace>      (predefined_functions);
   predefineFunction<ExprInsert>       (predefined_functions);
   predefineFunction<ExprJoin>         (predefined_functions);
   predefineFunction<ExprLength>       (predefined_functions);

   predefineFunction<ExprFunction>     (predefined_functions);
   predefineFunction<ExprEvaluateAt>   (predefined_functions);

   predefineFunction<ExprSet>          (predefined_functions);
   predefineFunction<ExprDefine>       (predefined_functions);

   predefineFunction<ExprBlock>        (predefined_functions);
   predefineFunction<ExprGlobal>       (predefined_functions);
   predefineFunction<ExprReturn>       (predefined_functions);
   predefineFunction<ExprBreak>        (predefined_functions);
   predefineFunction<ExprModule>       (predefined_functions);

   predefineFunction<ExprIf>           (predefined_functions);
   predefineFunction<ExprFor>          (predefined_functions);

   predefineFunction<ExprPlus>         (predefined_functions);
   predefineFunction<ExprSubtract>     (predefined_functions);
   predefineFunction<ExprMultiply>     (predefined_functions);
   predefineFunction<ExprDivide>       (predefined_functions);
   predefineFunction<ExprMod>          (predefined_functions);
   predefineFunction<ExprPower>        (predefined_functions);
   predefineFunction<ExprMinus>        (predefined_functions);

   predefineFunction<ExprNot>          (predefined_functions);
   predefineFunction<ExprAnd>          (predefined_functions);
   predefineFunction<ExprOr>           (predefined_functions);
   predefineFunction<ExprGreater>      (predefined_functions);
   predefineFunction<ExprLess>         (predefined_functions);
   predefineFunction<ExprEqual>        (predefined_functions);
   predefineFunction<ExprNotEqual>     (predefined_functions);
   predefineFunction<ExprGreaterEqual> (predefined_functions);
   predefineFunction<ExprLessEqual>    (predefined_functions);

   predefineFunction<ExprSin>          (predefined_functions);
   predefineFunction<ExprCos>          (predefined_functions);
   predefineFunction<ExprTan>          (predefined_functions);
   predefineFunction<ExprSinh>         (predefined_functions);
   predefineFunction<ExprCosh>         (predefined_functions);
   predefineFunction<ExprExp>          (predefined_functions);
   predefineFunction<ExprLog>          (predefined_functions);

   predefineFunction<ExprPrint>        (predefined_functions);

   predefineFunction<ExprIterator>     (predefined_functions);
   predefineFunction<ExprCreator>      (predefined_functions);

   predefineFunction<ExprImport>       (predefined_functions);
   predefineFunction<ExprToString>     (predefined_functions);

   predefineFunction<ExprRandom>       (predefined_functions);
   predefineFunction<ExprSeed>         (predefined_functions);

   predefineFunction<ExprRange>        (predefined_functions);

   predefineSymbol<ExprPi>          (predefined_symbols);
   predefineSymbol<ExprE>           (predefined_symbols);
   predefineSymbol<ExprGoldenRatio> (predefined_symbols);

   predefineSymbol<ExprAll>         (predefined_symbols);
   predefineSymbol<ExprAutomatic>   (predefined_symbols);
   predefineSymbol<ExprNone>        (predefined_symbols);
};

assertion<ExprParser::ParseError> ExprParser::Expect_expression(     ExpectExpression    );
assertion<ExprParser::ParseError> ExprParser::Expect_left_bracket(   ExpectLeftBracket   );
assertion<ExprParser::ParseError> ExprParser::Expect_right_bracket(  ExpectRightBracket  );
assertion<ExprParser::ParseError> ExprParser::Expect_right_brace(    ExpectRightBrace    );
assertion<ExprParser::ParseError> ExprParser::Expect_right_paren(    ExpectRightParen    );
assertion<ExprParser::ParseError> ExprParser::Expect_right_ref(      ExpectRightRef      );
assertion<ExprParser::ParseError> ExprParser::Expect_right_underline(ExpectRightUnderline);
assertion<ExprParser::ParseError> ExprParser::Expect_right_string(   ExpectRightString   );
assertion<ExprParser::ParseError> ExprParser::Expect_end(            ExpectEnd           );


std::string ExprParser::print_error(char const* in, char const* pos)
{
   // get input lines above and below error
   const int inputlines = 4;

   char const* begin = in;
   char const* before = pos;
   char const* after  = pos;
   char const* end    = pos;
   char const* it = pos;
   unsigned int lns = 0; unsigned int lnscnt = 0; unsigned int arr = 0; 
   while (it != begin)
   {
      if ( *it=='\n' ) lns++;
      if ( lns <= inputlines ) { before--;}
      if (lns == 0) arr++;
      it--;
   }
   while (*after != '\0')
   {
      if ( *after=='\n' ) lnscnt++;
      if ( lnscnt > inputlines ) { break; }
      if ( lnscnt == 0 ) end++;
      after++;
   }
   //end++;

   std::ostringstream str;
   str << "Error occured at ^ in line " << lns+1 << " of input:" << std::endl;
   str << "--> |" << std::endl;
   str << std::string(before, end) << std::endl;
   str << setw(arr) << "^";
   str << std::string(end, after) << std::endl;
   str << "| <--" << std::endl;

   return str.str();
};

bool ExprParser::parse_file(const char* filename, std::string& errstr)
{
   PARSER_DEBUG("ExprParser.parse()", "")
   PARSER_DEBUG("parse file:", filename)

   std::ostringstream str;

   std::ifstream file(filename, std::ios::in|std::ios::binary|std::ios::ate);
   std::ifstream::pos_type size;
   char * input = 0;
   char * last = 0;
   if (file.is_open())
   {
      size = file.tellg();
      input = new char [1 + size];
      file.seekg (0, std::ios::beg);
      file.read (input, size);
      file.close();
      input[size] = '\0';
      last = input + size;

      PARSER_DEBUG("Input is:", std::string("\"") + std::string(input, last) + "\"")
   }
   else
   {
      str << "Cannot open file: " << filename << " !\n";
      errstr = str.str();
      return false;
   }
   return parse(input, last, errstr);
}

bool ExprParser::parse(const std::string& str, std::string& errstr)
{
   return parse(str.c_str(), str.c_str() + str.length(), errstr);
}


bool ExprParser::parse(const char* input, const char* last, std::string& errstr)
{
   std::ostringstream str;

   SimGrammar sim_grammar(expr_stack, scope_stack, predefined_functions, predefined_symbols);
   CommentGrammar comment_grammar;

   parse_info<> pinfo; 

   try {
      pinfo = boost::spirit::parse(input, last, sim_grammar, comment_grammar);
   } catch (parser_error<ExprParser::ParseError, char const*>& e) {
      std::map<ParseError, std::string>  errmsg;
      errmsg[ExpectExpression]     = "an expression";
      errmsg[ExpectLeftBracket]    = "\']\'";
      errmsg[ExpectRightBrace]     = "\'}\'";
      errmsg[ExpectRightBracket]   = "\']\'";
      errmsg[ExpectRightParen]     = "\')\'";
      errmsg[ExpectRightRef]       = "\']]\'";
      errmsg[ExpectRightUnderline] = "\'_\'";
      errmsg[ExpectAssign]         = "\'=\'";
      errmsg[ExpectDef]            = "\':=\'";
      errmsg[ExpectEnd]            = "\';\'";

      //std::ostringstream str;
      str << "Syntax Error: ";
      str << "Missing " << errmsg[e.descriptor] << endl; 
      str << print_error(input, e.where);
      errstr = str.str();
      PARSER_DEBUG(str.str(), "");
      return false;
   }
   catch (parser_error<ExprSyntaxErrorT, const char*>& e)
   {
      //std::ostringstream str;
      str << "Syntax Error: ";
      str << ExprSyntaxErrorDescription(e.descriptor) << "!" << endl;
      str << print_error(input, e.where);
      errstr = str.str();
      PARSER_DEBUG(str.str(), "");
      return false;
   };

   if (!pinfo.full)
   {
      str << "Syntax Error!";
      str << print_error(input, pinfo.stop);
      errstr = str.str();
      PARSER_DEBUG(str.str(), "");
      return false;
   };

   result = expr_stack.top();

   PARSER_DEBUG("Parsing succes! parsed input to:", result)

   return true;
};



