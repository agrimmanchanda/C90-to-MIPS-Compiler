#ifndef ast_statements_hpp
#define ast_statements_hpp

#include "ast.hpp"

extern int scopelevel;
extern int loop_count;
extern int if_level;
extern bool loop_while;
extern bool loop_for;

class AssignmentStatement: public Expression {
public:
    std::string VarName;
    ExpressionPtr Expression;

    ~AssignmentStatement() {}

    AssignmentStatement (std::string _VarName, ExpressionPtr _Expression)
     : VarName(_VarName), Expression(_Expression) {}

    virtual void print(std::ostream &dst) const override
    {
        dst << VarName << " = ";
        Expression->print(dst);
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << VarName << " = ";
        Expression->translate(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        int stack_end = (var_count*4) +parameter_count+12+50;

        //find a free register
  
        Expression->compile(dst, contxt, destReg);
        contxt.store_var_val(VarName, Expression->evaluate());

        //store result into variable
        if (contxt.LookupVariable(VarName, scopelevel) != 0)
            dst<<"\t"<<"sw"<<"\t"<<"$"<<destReg<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#Assign variable "<<VarName<<std::endl;

        else if (contxt.find_parameter(VarName) > -1) {
            dst<<"\t"<<"sw"<<"\t"<<"$"<<destReg<<", "<<(contxt.find_parameter(VarName))*4+stack_end<<"($fp)"<<"\t#Assign passed variable "<<VarName<<std::endl;
        }
        // if(varGlobal == false){
        //     dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<stack_end<<"($fp)"<<"\t#Assign function result"<<VarName<<std::endl;
        // }
        // else{
        //     dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#Assign variable "<<VarName<<std::endl;
        // }

    }
};

class AddAssignmentStatement: public Expression {
public:
    std::string VarName;
    ExpressionPtr Expression;

    ~AddAssignmentStatement() {}

    AddAssignmentStatement (std::string _VarName, ExpressionPtr _Expression)
     : VarName(_VarName), Expression(_Expression) {}

    virtual void print(std::ostream &dst) const override
    {
        dst << VarName << " += ";
        Expression->print(dst);
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << VarName << " += ";
        Expression->translate(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        //find a free registers
        std::vector<int> freeRegs = contxt.FindFreeTempRegs();
        contxt.set_used(freeRegs[0]);
        contxt.set_used(freeRegs[1]);
        Expression->compile(dst, contxt, freeRegs[0]);

        //load result into free reg 1
        dst<<"\t"<<"lw"<<"\t"<<"$"<<freeRegs[1]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#AddAssign loading variable: "<<VarName<<std::endl;

        //increment reg 1 = reg 1 + reg 0
        dst<<"\t"<<"addu"<<"\t"<<"$"<<freeRegs[0]<<", $"<<freeRegs[1]<<", $"<<freeRegs[0]<<"\t #Incrementing variable: "<<VarName<<std::endl;

        //load result into reg 0
        dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#AddAssign storing variable: "<<VarName<<std::endl;

        contxt.set_unused(freeRegs[0]);
        contxt.set_unused(freeRegs[1]);
    }
};

class SubAssignmentStatement: public Expression {
public:
    std::string VarName;
    ExpressionPtr Expression;

    ~SubAssignmentStatement() {}

    SubAssignmentStatement (std::string _VarName, ExpressionPtr _Expression)
     : VarName(_VarName), Expression(_Expression) {}

    virtual void print(std::ostream &dst) const override
    {
        dst << VarName << " -= ";
        Expression->print(dst);
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << VarName << " -= ";
        Expression->translate(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        //find a free registers
        std::vector<int> freeRegs = contxt.FindFreeTempRegs();
        contxt.set_used(freeRegs[0]);
        contxt.set_used(freeRegs[1]);
        Expression->compile(dst, contxt, freeRegs[0]);

        //load result into free reg 1
        dst<<"\t"<<"lw"<<"\t"<<"$"<<freeRegs[1]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#SubAssign loading variable: "<<VarName<<std::endl;

        //decrement reg 0 = reg 1 - reg 0
        dst<<"\t"<<"sub"<<"\t"<<"$"<<freeRegs[0]<<", $"<<freeRegs[1]<<", $"<<freeRegs[0]<<"\t #Decrementing variable: "<<VarName<<std::endl;

        //load result into reg 0
        dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#SubAssign storing variable: "<<VarName<<std::endl;

        contxt.set_unused(freeRegs[0]);
        contxt.set_unused(freeRegs[1]);
    }
};

class MulAssignmentStatement: public Expression {
public:
    std::string VarName;
    ExpressionPtr Expression;

    ~MulAssignmentStatement() {}

    MulAssignmentStatement (std::string _VarName, ExpressionPtr _Expression)
     : VarName(_VarName), Expression(_Expression) {}

    virtual void print(std::ostream &dst) const override
    {
        dst << VarName << " *= ";
        Expression->print(dst);
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << VarName << " *= ";
        Expression->translate(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        //find a free registers
        std::vector<int> freeRegs = contxt.FindFreeTempRegs();
        contxt.set_used(freeRegs[0]);
        contxt.set_used(freeRegs[1]);
        Expression->compile(dst, contxt, freeRegs[0]);

        //load result into free reg 1
        dst<<"\t"<<"lw"<<"\t"<<"$"<<freeRegs[1]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#MulAssign loading variable: "<<VarName<<std::endl;

        //increment reg 1 = reg 1 * reg 0
        dst<<"\t"<<"mult"<<"\t"<<"$"<<freeRegs[0]<<", $"<<freeRegs[1]<<"\t #Multiplying variable: "<<VarName<<std::endl;
        dst<<"\t"<<"mflo"<<"\t"<<"$"<<destReg<<std::endl;

        //load result into reg 0
        dst<<"\t"<<"sw"<<"\t"<<"$"<<destReg<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#MulAssign storing variable: "<<VarName<<std::endl;

        contxt.set_unused(freeRegs[0]);
        contxt.set_unused(freeRegs[1]);
    }
};

class DivAssignmentStatement: public Expression {
public:
    std::string VarName;
    ExpressionPtr Expression;

    ~DivAssignmentStatement() {}

    DivAssignmentStatement (std::string _VarName, ExpressionPtr _Expression)
     : VarName(_VarName), Expression(_Expression) {}

    virtual void print(std::ostream &dst) const override
    {
        dst << VarName << " /= ";
        Expression->print(dst);
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << VarName << " /= ";
        Expression->translate(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        //find a free registers
        std::vector<int> freeRegs = contxt.FindFreeTempRegs();
        contxt.set_used(freeRegs[0]);
        contxt.set_used(freeRegs[1]);
        Expression->compile(dst, contxt, freeRegs[0]);

        //load result into free reg 1
        dst<<"\t"<<"lw"<<"\t"<<"$"<<freeRegs[1]<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#DivAssign loading variable: "<<VarName<<std::endl;

        //divide reg 1 = reg 1 / reg 0
        dst<<"\t"<<"div"<<"\t"<<"$"<<freeRegs[1]<<", $"<<freeRegs[0]<<"\t #Dividing variable: "<<VarName<<std::endl;
        dst<<"\t"<<"mflo"<<"\t"<<"$"<<destReg<<std::endl;

        //load result into reg 0
        dst<<"\t"<<"sw"<<"\t"<<"$"<<destReg<<", "<<contxt.LookupVariable(VarName, scopelevel)<<"($fp)"<<"\t#DivAssign storing variable: "<<VarName<<std::endl;

        contxt.set_unused(freeRegs[0]);
        contxt.set_unused(freeRegs[1]);
    }
};


class ArrayAssignment : public Expression {
private:
    std::string Identifier;
    // int index_no;
    ExpressionPtr Expression;
    ExpressionPtr IndexExpr;
public:
    ~ArrayAssignment() {}
    // ArrayAssignment(std::string _Identifier, int _index_no, ExpressionPtr _Expression)
    //  : Identifier(_Identifier), index_no(_index_no), Expression(_Expression), IndexExpr(NULL) {}
    ArrayAssignment(std::string _Identifier, ExpressionPtr _IndexExpr, ExpressionPtr _Expression)
     : Identifier(_Identifier), Expression(_Expression), IndexExpr(_IndexExpr) {}


    virtual void print (std::ostream &dst) const override {
        dst<<Identifier<<"[";
        IndexExpr->print(dst);
        dst<<"] = ";
        Expression->print(dst);
        dst<<";";
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        std::vector<int> freeRegs = contxt.FindFreeTempRegs();
        contxt.set_used(freeRegs[0]);
        int index_no = IndexExpr->evaluate();
        Expression->compile(dst, contxt, freeRegs[0]);

        int var_offset = contxt.LookupVariable(Identifier + std::to_string(index_no), scopelevel);
        if (var_offset !=0) {
            dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", "<<var_offset<<"($fp)"<<"\t#Assign element "<<index_no<<" of array "<<Identifier<<std::endl;
        }
        else {
            //global array
            dst<<"\t"<<"lui"<<"\t"<<"$"<<destReg<<", "<<"%hi("<<Identifier<<")"<<"\t#Loading in array: "<<Identifier<<std::endl;
            dst<<"\t"<<"addiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", %lo("<<Identifier<<")"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
            dst<<"\t"<<"addiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", "<<index_no*4<<"\t#Add offset for element "<<index_no<<std::endl;
            dst<<"\t"<<"sw"<<"\t"<<"$"<<freeRegs[0]<<", ($"<<destReg<<")"<<"\t#Store into global array "<<Identifier<<std::endl;
        }
        dst<<"\t"<<"nop"<<std::endl;
        contxt.set_unused(freeRegs[0]);
    }
};

class ReturnStatement : public Expression {
public:
    ExpressionPtr AdditionalExpressions;

    ~ReturnStatement() {}

    ReturnStatement (ExpressionPtr _AdditionalExpressions): AdditionalExpressions(_AdditionalExpressions) {}

    virtual void print(std::ostream &dst) const override {
        dst<<"return";
        if (AdditionalExpressions != NULL) {
            dst<<" ";
            AdditionalExpressions->print(dst);
            dst<<";";
        }
        else {
            dst<<";";
        }
    }

    virtual void translate(std::ostream &dst) const override {
        dst << "return";
        if (AdditionalExpressions != NULL) {
            dst << " ";
            AdditionalExpressions->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {

        if (AdditionalExpressions != NULL) {
            AdditionalExpressions->compile(dst, contxt, destReg);
            //branch to end of function label
        }
        dst<<"\t"<<"b "<<function_def_queue.back()<<"_function_end_"<<function_def_num<<"\t#Return statement"<<std::endl;
        if (function_def_queue.back()=="main") {
            main_returned = true;
        }
    }
};

class IfElseStatement : public Expression {
private:
    ExpressionPtr Condition;
    NodePtr IBody;
    NodePtr EBody;

public:
    ~IfElseStatement () {}

    IfElseStatement (ExpressionPtr _Condition, NodePtr _IBody, NodePtr _EBody) : Condition(_Condition), IBody(_IBody), EBody(_EBody) {}

    virtual void print (std::ostream &dst) const override {
        dst << "if (";
        Condition->print(dst);
        dst<<") ";
        IBody->print(dst);
        if (EBody != NULL) {
            dst<<std::endl;
            for (int i = 0; i < scopelevel; i++) {dst<<"\t";}
            dst<<"else";
            EBody->print(dst);
        }

    }

    virtual void translate(std::ostream &dst) const override {
        dst<< "if(";
        Condition->translate(dst);
        dst<<"):";
        IBody->translate(dst);

        if(EBody != NULL){
            dst<<std::endl;
            for(int i = 0; i < scopelevel; i++){dst<<"\t";}
            dst<<"else:";
            EBody->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> FreeReg = contxt.FindFreeRegs(8, 15);
        contxt.set_used(FreeReg[0]);
        int current_if_level = if_level++;
        Condition->compile(dst, contxt, FreeReg[0]);

        dst << "\t"<<"beq"<<"\t" << "$0, $" << FreeReg[0] << ", else_"<<current_if_level << std::endl; //$else condition yet to be filled
		dst << "\t"<<"nop"<<"\t" << std::endl;
		contxt.set_unused(FreeReg[0]);

        if(IBody != NULL){
            IBody->compile(dst, contxt, destReg);
        }

        dst<<"\t"<<"b"<<"\t"<<"ifelse_end_"<<current_if_level<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;

        dst<<"else_"<<current_if_level<<":"<<std::endl;
        if(EBody != NULL){
            EBody->compile(dst, contxt, destReg);
        }

        dst<<"ifelse_end_"<<current_if_level<<":"<<std::endl;
        
    }
};

class SwitchBody : public AST_node {
  private:
    std::string name;
    ExpressionPtr CaseExpression;
    NodePtr Statements;
    NodePtr Next_case;

  public:
    ~SwitchBody() {}
    SwitchBody(std::string _name, ExpressionPtr _CaseExpression, NodePtr _Statements, NodePtr _Next_case)
     : name(_name), CaseExpression(_CaseExpression), Statements(_Statements), Next_case(_Next_case) {}

    virtual void print(std::ostream &dst) const override {
        for (int i = 0; i < scopelevel; i++) {dst << "\t";}
        dst<<name<<" ";
        if (CaseExpression!=NULL)
            CaseExpression->print(dst);
        dst<<":"<<std::endl;
        // for (int i = 0; i < scopelevel; i++) {dst << "\t";}
        Statements->print(dst);
        if (Next_case!=NULL)
            Next_case->print(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

    }
};

class ScopeBody : public Expression {
private:
    NodePtr Body;
public:
    ~ScopeBody () {}

    ScopeBody (NodePtr _Body) : Body(_Body) {}

    virtual void print(std::ostream &dst) const override {
        dst << "{ " << std::endl;
        // std::cout<<"scope level: "<<scopelevel<<std::endl;
        scopelevel++;
        if (Body!=NULL)
            Body->print(dst);
        scopelevel--;
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        dst << "}";
    }

    virtual void translate(std::ostream &dst) const override {
        dst << std::endl;
        scopelevel++;
        if (Body != NULL)
            Body->translate(dst);
        scopelevel--;
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        //dst << std::endl;
        // scopelevel++;
        dst<<"#FUNCTION BODY"<<std::endl;
        if (Body != NULL)
            Body->compile(dst, contxt, destReg);
        // scopelevel--;
        // for (int i = 0; i < scopelevel; i++) {
        //     dst << "\t";
        // }
    }
};

// class NestedScope : public Expression {
//   private:
//     NodePtr Scope;
//   public:
//     ~NestedScope() {}
//     NestedScope (NodePtr _Scope) : Scope(_Scope) {}

//     virtual void print (std::ostream &dst) const override {
//         Scope->print(dst);
//     }

//     virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
//         Scope->compile(dst,contxt,destReg);
//     }
// };

class NoBraces : public Expression
{
  private:
    NodePtr Body;

  public:
    ~NoBraces() {}

    NoBraces(NodePtr _Body) : Body(_Body) {}

    virtual void print(std::ostream &dst) const override
    {
        // std::cout<<"scope level: "<<scopelevel<<std::endl;
        scopelevel++;
        for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }
        Body->print(dst);
        scopelevel--;
                for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }

    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << std::endl;
        scopelevel++;
        for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }
        Body->translate(dst);
        scopelevel--;
        for (int i = 0; i < scopelevel; i++)  { dst << "\t"; }

    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        Body->compile(dst,contxt,destReg);
    }
};

class ScopeStatements: public AST_node {
private:
    NodePtr Singular_statement;
    NodePtr Rest_of_statements;
public:
    ~ScopeStatements () {}

    ScopeStatements (NodePtr _Singular_statement, NodePtr _Rest_of_statements)
     : Singular_statement(_Singular_statement), Rest_of_statements(_Rest_of_statements) {}

    virtual void print(std::ostream &dst) const override {
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        Singular_statement->print(dst);
        dst<<std::endl;
        if (Rest_of_statements != NULL) {
            Rest_of_statements->print(dst);
        }
    }

    virtual void translate(std::ostream &dst) const override {
        for (int i = 0; i < scopelevel; i++) {
            dst << "\t";
        }
        Singular_statement->translate(dst);
        dst<<std::endl;
        if (Rest_of_statements != NULL) {
            Rest_of_statements->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        // for (int i = 0; i < scopelevel; i++) {
        //     dst << "\t";
        // }
        Singular_statement->compile(dst, contxt, destReg);
        if (Rest_of_statements != NULL) {
            Rest_of_statements->compile(dst, contxt, destReg);
        }
    }


};

class BracketedExpr : public Expression {
private:
    ExpressionPtr Expression;
public:
    ~BracketedExpr() {}
    BracketedExpr(ExpressionPtr _Expression) : Expression(_Expression) {}

    virtual void print (std::ostream &dst) const override {
        dst<<"(";
        Expression->print(dst);
        dst<<")";
    }

    virtual void translate (std::ostream &dst) const override {
        dst<<"(";
        Expression->translate(dst);
        dst<<")";
    }

    virtual int evaluate () const override {
        return Expression->evaluate();
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        Expression->compile(dst, contxt, destReg);
    }
};

class SwitchStatement: public Expression {
private:
    ExpressionPtr Condition;
    NodePtr Body;

public:
    ~SwitchStatement() {}

    SwitchStatement (ExpressionPtr _Condition, NodePtr _Body) : Condition(_Condition) , Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        dst<<"switch(";
        Condition->print(dst);
        dst <<") {"<<std::endl;
        // dst<<"\t";
        scopelevel++;
        Body->print(dst);
        scopelevel--;
        for (int i = 0; i < scopelevel; i++) {dst << "\t";}
        dst<<"}"<<std::endl;
    }
    virtual void compile(std::ostream &dst, Context &contxt, int destReg){
        Condition->compile(dst, contxt, destReg);
        Body->compile(dst, contxt, destReg);
    }
};

class BreakStatement : public Expression {
private:

public:
    ~BreakStatement() {}
    BreakStatement() {}

    virtual void print (std::ostream &dst) const override {
        dst<<"break;"<<std::endl;
    }

    virtual void translate (std::ostream &dst) const override {
        dst<<"break"<<std::endl;
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        dst<<"\t"<<"b"<<"\t"<<loop_ends.back()<<"\t #Break called"<<std::endl;
    }
};

class ContinueStatement : public Expression {
private:

public:
    ~ContinueStatement() {}
    ContinueStatement() {}

    virtual void print (std::ostream &dst) const override {
        dst<<"continue;"<<std::endl;
    }

    virtual void translate (std::ostream &dst) const override {
        dst<<"continue"<<std::endl;
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {

        if(loop_while == true){
            dst<<"\t"<<"b"<<"\t"<<"while_loop_"<<loop_count-1<<"_begin"<<"\t #while continue statement"<<std::endl;
        }
        else if(loop_for == true){
            dst<<"\t"<<"b"<<"\t"<<"for_increment_"<<loop_count<<"\t #for continue statement"<<std::endl;
            //dst<<"\t"<<"b"<<"\t"<<"ifelse_end_"<<loop_count-1<<"\t #for continue statement"<<std::endl;
        }
    }
};


#endif
