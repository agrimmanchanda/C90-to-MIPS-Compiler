%option noyywrap

%{
/* Now in a section of C that will be embedded
   into the auto-generated code. Flex will not
   try to interpret code surrounded by %{ ... %} */

/* Bring in our declarations for token types and
   the yylval variable. */

#include "c_parser.tab.hpp"
#include <iostream>

// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

/* End the embedded code section. */

%}


%%

"//".* {}  /*Ignore single line comments, credits: https://stackoverflow.com/questions/25395251/detecting-and-skipping-line-comments-with-flex */

[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/] {}  /*Ignore multi-line comments, credits: https://stackoverflow.com/questions/25395251/detecting-and-skipping-line-comments-with-flex */

"int" 			    { yylval.string = new std::string(yytext); return T_INT; }
"double"        { yylval.string = new std::string(yytext); return T_DOUBLE; }
"float"         { yylval.string = new std::string(yytext); return T_FLOAT; }
"long"          { yylval.string = new std::string(yytext); return T_LONG; }
"void"          { yylval.string = new std::string(yytext); return T_VOID; }
"char"          { yylval.string = new std::string(yytext); return T_CHAR; }
"short"         { yylval.string = new std::string(yytext); return T_SHORT; }
"signed"        { yylval.string = new std::string(yytext); return T_SIGNED; }
"if"            { yylval.string = new std::string(yytext); return T_IF; }
"else"          { yylval.string = new std::string(yytext); return T_ELSE; }
"while"         { yylval.string = new std::string(yytext); return T_WHILE; }
"for"           { yylval.string = new std::string(yytext); return T_FOR; }
"do"            { yylval.string = new std::string(yytext); return T_DO; }
"switch"        { yylval.string = new std::string(yytext); return T_SWITCH; }
"case"          { yylval.string = new std::string(yytext); return T_CASE; }
"default"       { yylval.string = new std::string(yytext); return T_DEFAULT; }
"break"			    { yylval.string = new std::string(yytext); return T_BREAK; }
"continue"		  { yylval.string = new std::string(yytext); return T_CONTINUE; }
"go to"			    { yylval.string = new std::string(yytext); return T_GO_TO; }
"auto"          { yylval.string = new std::string(yytext); return T_AUTO; }
"struct"        { yylval.string = new std::string(yytext); return T_STRUCT; }
"return"        { yylval.string = new std::string(yytext); return T_RETURN; }
"enum"          { yylval.string = new std::string(yytext); return T_ENUM; }

[(]             { return T_LBRACKET; }
[{]             { return T_CURLY_LBRACKET; }
\[              { return T_SQUARE_LBRACKET; }

[)]             { return T_RBRACKET; }
[}]             { return T_CURLY_RBRACKET; }
\]              { return T_SQUARE_RBRACKET; }


[*]             { return T_TIMES; }
[+]             { return T_PLUS; }
[-]             { return T_MINUS; }
[/]             { return T_DIVIDE; }
[%]             { return T_MODULUS; }

[=]             { return T_EQUAL; }
[+][=]          { return T_PLUS_EQUAL; }
[-][=]          { return T_MINUS_EQUAL; }
[*][=]          { return T_TIMES_EQUAL; }
[/][=]          { return T_DIVIDE_EQUAL; }
[%][=]          { return T_MODULUS_EQUAL; }

[=][=]          { return T_IS_EQUAL; }
[!][=]          { return T_IS_NOT_EQUAL; }
\<             { return T_LESS_THAN; }
[<][=]          { return T_LESS_EQUAL_THAN; }
[=][<]          { return T_LESS_EQUAL_THAN; }
\>             { return T_GREATER_THAN; }
[>][=]          { return T_GREATER_EQUAL_THAN; }
[=][>]          { return T_GREATER_EQUAL_THAN; }

[|][|]          { return T_LOGICAL_OR; }
[&][&]          { return T_LOGICAL_AND; }
[!]             { return T_NOT; }

["?"]           { return T_TERNARY; }
[&]             { return T_BITWISE_AND; }
[|]             { return T_BITWISE_OR; }
["^"]           { return T_BITWISE_XOR; }
[~]             { return T_BITWISE_COMPLEMENT; }
[<][<]          { return T_LSHIFT; }
[>][>]          { return T_RSHIFT; }


[;]				      { return T_SEMICOLON; }
[:]				      { return T_COLON; }
[,]				      { return T_COMMA; }

[+][+]          { return T_INCREMENT; }
[-][-]          { return T_DECREMENT; }

[0-9]+([.][0-9]*)?([Ee][-+]?[0-9]+)? { yylval.number=strtod(yytext, 0); return T_NUMBER; }

[0][Xx][0-9a-fA-F]+                   {yylval.number=strtod(yytext, 0); return T_NUMBER; }

[a-zA-Z_]+[a-z0-9_]*        { yylval.string=new std::string(yytext); return T_VARIABLE; }

["][a-z]+[a-z0-9]*["]    { yylval.string=new std::string(yytext); return T_STRING; }

[ \n\t]         {;}
.               { fprintf(stderr, "Invalid token\n"); exit(1); }

%%

/* Error handler. This will get called if none of the rules match. */
void yyerror (char const *s)
{
  fprintf (stderr, "Parse Error: %s\n", s); /* s is the text that wasn't matched */
  exit(1);
}
