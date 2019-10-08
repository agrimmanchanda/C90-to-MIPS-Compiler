#include "../include/ast.hpp"

int main () {
    //Lexer debugging 
    extern int yy_flex_debug;
    yy_flex_debug = 0;  //set to 1 to debug

    const AST_node *ast=parseAST();

    ast->translate(std::cout);
    std::cout<<std::endl;

    
    return 0;
}