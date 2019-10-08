#include "../include/ast.hpp"
#include <stdio.h>
#include <string>


int main(int argc, char *argv[])
{
    //Lexer debugging
    extern int yy_flex_debug;
    yy_flex_debug = 0;  //set to 1 to debug

  	freopen (argv[2],"r",stdin);

    const AST_node *ast=parseAST();
  	std::cout<< std::endl;

  	freopen (argv[4],"w",stdout);
  	std::string argument = argv[1];

  	if (argument == "--translate") {
  	  ast->translate(std::cout);
      std::cout<<std::endl;
  	}
  	else if (argument=="-S") {
  		Context *contxt = new Context();
  		ast->compile(std::cout, *contxt, 2);
  		std::cout<<std::endl;
  	}

  	fclose(stdin);
  	fclose(stdout);
  return 0;
}
