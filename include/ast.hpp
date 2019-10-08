#ifndef ast_hpp
#define ast_hpp

#include "ast/ast_context.hpp"
#include "ast/ast_node.hpp"
#include "ast/ast_expression.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_operators.hpp"
#include "ast/ast_functions.hpp"
#include "ast/ast_dec_def.hpp"
#include "ast/ast_statements.hpp"
#include "ast/ast_variables.hpp"
#include "ast/ast_enum.hpp"
#include "ast/ast_loops.hpp"

extern const AST_node *parseAST();

#endif
