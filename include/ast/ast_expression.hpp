#ifndef ast_expression_hpp
#define ast_expression_hpp

#include "ast_node.hpp"

#include <string>
#include <iostream>
#include <map>
#include <memory>



class Expression;

typedef const Expression *ExpressionPtr;

static std::vector<std::string> GlobalNames;
static std::vector<ExpressionPtr> GlobalNameExpr;
static std::vector<std::string> LocalNames;
static std::vector<ExpressionPtr> LocalNameExpr;

class Expression : public AST_node
{
public:
    virtual ~Expression()
    {}

    //! Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst) const =0;

    //! Evaluate the tree using the given mapping of variables to numbers
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const
    { throw std::runtime_error("Expr evaluate not implemented."); }

    virtual int evaluate () const {
        throw std::runtime_error("Eval Not implemented");
    }
};


#endif
