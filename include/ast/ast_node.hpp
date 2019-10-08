#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include "ast_context.hpp"

static std::vector<std::string> global_vars;
static int var_count;
static int scopelevel;
static std::vector<std::string> function_call_queue;
static std::vector<std::string> loop_ends;
static int parameter_count = 16;
static int function_call_num;
static bool varGlobal;
static int loop_count;
static int if_level;
static bool main_returned;
static std::string expr_var;

// static int index_no;
static int enum_list_size;
static bool loop_while = false;
static bool loop_for = false;


class AST_node;

typedef const AST_node *NodePtr;

class AST_node: public Context
{
public:
    virtual ~AST_node()
    {}

    //! Tell a node to print itself to the given stream
    virtual void print(std::ostream &dst) const =0;

    //! Evaluate the tree using the given mapping of variables to numbers
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const
    { throw std::runtime_error("Evaluate not implemented."); }

    virtual void translate(std::ostream &dst) const {
        throw std::runtime_error("Translate not implemented.");
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const
    {
        throw std::runtime_error("Compilation not implemented.");
    }

};


#endif
