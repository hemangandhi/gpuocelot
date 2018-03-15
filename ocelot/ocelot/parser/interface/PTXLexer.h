/*!
	\file PTXLexer.h
	\date Monday January 19, 2009
	\author Gregory Diamos <gregory.diamos@gatech.edu>
	\brief The header file for the PTXLexer class.
*/

#ifndef PTX_LEXER_H_INCLUDED
#define PTX_LEXER_H_INCLUDED

//#include <ptxgrammar.hpp>
union YYSTYPE
{
#line 45 "ocelot/ocelot/parser/implementation/ptxgrammar.yy" /* yacc.c:1909  */

	char text[1024];
	long long int value;
	long long unsigned int uvalue;

	double doubleFloat;
	float singleFloat;

#line 342 ".release_build/ocelot/ptxgrammar.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;

namespace parser
{
	/*!	\brief A wrapper around yyFlexLexer to allow for a local variable */
	class PTXLexer : public ptxFlexLexer
	{
		public:
			YYSTYPE*     yylval;
			int          column;
			int          nextColumn;

		public:
			PTXLexer( std::istream* arg_yyin = 0, 
				std::ostream* arg_yyout = 0 );
	
			int yylex();
			int yylexPosition();
			
		public:
			static std::string toString( int token );
	
	};

}

#endif

