#include "../include/ast.hpp"

int main () {
    //Lexer debugging 
    extern int yy_flex_debug;
    yy_flex_debug = 0;  //set to 1 to debug

    const AST_node *ast=parseAST();

    Context* contxt = new Context();
    ast->compile(std::cout, *contxt, 2);
    std::cout<<std::endl;

    
    return 0;
}