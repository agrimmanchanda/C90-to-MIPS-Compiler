#ifndef ast_functions_hpp
#define ast_functions_hpp

#include "ast_expression.hpp"
#include <cmath>

extern std::vector<std::string> function_call_queue;
extern int function_call_num;

class FunctionCall : public Expression
{
  private:
    std::string name;
    ExpressionPtr arg;


  public:
    FunctionCall(std::string _name, ExpressionPtr _arg) : name(_name), arg(_arg) {     
    }
    ~FunctionCall() {}

    virtual void print(std::ostream &dst) const override {
        dst << name << "(";
        if (arg != NULL) {
            arg->print(dst);
        }
        
        dst << ")";
    }

    virtual void translate(std::ostream &dst) const {
        dst << name << "(";
        if (arg != NULL) {
            arg->translate(dst);
        }

        dst << ")";
    }

    virtual int evaluate () const override {
        //function call cant really implement
        return 0;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        function_call_num++;
        function_call_queue.push_back(name);
        std::vector<int> lockedRegs = contxt.FindLockedTempRegs();
        int stack_count = localvar_counter;
        
        //store temp registers before function call in stack
        for (unsigned int i = 0; i < lockedRegs.size();i++) {
            stack_count++;
            dst << "\t"<<"sw"<< "\t"<< "$"<<lockedRegs[i]<<", "<<(stack_count)*4+16<< "($fp)";
            dst <<"\t#Storing temp register: "<<lockedRegs[i]<< std::endl;
        }

        if (arg!=NULL)
            arg->compile(dst,contxt,destReg);

        //function call
        dst<<"\t"<<"jal"<<"\t"<<name<<"\t#Function called"<<std::endl;

        //restore temp registers
        for ( int i = lockedRegs.size()-1; i >= 0;i--) {
            dst << "\t"<<"lw"<< "\t"<< "$"<<lockedRegs[i]<<", "<<(stack_count)*4+16<< "($fp)";
            dst <<"\t#Loading temp register: "<<lockedRegs[i]<< std::endl;
            dst<<"\t"<<"nop"<<std::endl;
            stack_count--;
        }
    }
};

class Args: public Expression {
private:
    NodePtr SingleParameter;
    NodePtr OtherParameters;
public:
    ~Args() {}

    Args(NodePtr _SingleParameter, NodePtr _OtherParameters) : SingleParameter(_SingleParameter), OtherParameters(_OtherParameters) {}

    virtual void print (std::ostream &dst) const override {
        SingleParameter->print(dst);
        if (OtherParameters != NULL) {
            dst << ",";
            OtherParameters->print(dst);
        }
    }

    virtual void translate (std::ostream &dst) const override {
        SingleParameter->translate(dst);
        if (OtherParameters != NULL) {
            dst << ",";
            OtherParameters->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        SingleParameter->compile(dst, contxt, destReg);
        if (OtherParameters != NULL) {
            OtherParameters->compile(dst, contxt, destReg);
        }
    }

};

class Parameter: public Expression {
private:
    std::string Type;
    std::string Identifier;

public:
    ~Parameter () {}

    Parameter(std::string _Type, std::string _Identifier) : Type(_Type), Identifier(_Identifier) {}

    virtual void print (std::ostream &dst) const override {
        dst << Type <<" "<< Identifier;
    }

    virtual void translate (std::ostream &dst) const override {
        dst<< Identifier;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        contxt.set_parameter_list(Identifier);

        // if(destReg < 8){  //check that we only have argument registers
        //     //looks for local variables in the scopes
        //     dst<<"\t"<<"sw"<<"\t"<<"$"<<freeArgReg[0]<<", "<<var_offset<<"($fp)"<<"\t"<<"#storing param argument register"<<std::endl;
        // }
        // destReg++; //increment destReg incase we have more than one parameter.

        // for(int i =0; i<4; i++){
        //     contxt.set_unused(freeArgReg[i]);
        // }
        
    } 
};

class PassedParams : public Expression {
private:
    ExpressionPtr CurrentParam;
    ExpressionPtr OtherParams;

public:
    ~PassedParams() {}

    PassedParams (ExpressionPtr _CurrentParam, ExpressionPtr _OtherParams) : CurrentParam(_CurrentParam), OtherParams(_OtherParams) {}

    virtual void print (std::ostream &dst) const override {
        CurrentParam->print(dst);
        if (OtherParams != NULL) {
            dst << ",";
            OtherParams->print(dst);
        }
    }

    virtual void translate (std::ostream &dst) const override {
        CurrentParam->translate(dst);
        if (OtherParams != NULL) {
            dst << ",";
            OtherParams->translate(dst);
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {

        std::vector<int> freeParamReg = contxt.FindFreeRegs(4, 7);
        contxt.set_used(freeParamReg[0]);

        CurrentParam->compile(dst, contxt, destReg);

        dst<<"\t"<<"move"<<"\t"<<"$"<<freeParamReg[0]<<", $"<<destReg<<"\t #move param to arg reg"<<std::endl;

        if (OtherParams != NULL) {
            OtherParams->compile(dst, contxt, destReg);
        }

        contxt.set_unused(freeParamReg[0]);
    } 
};

class Function: public Expression {
private:
    ExpressionPtr arg;
protected:
    Function(ExpressionPtr _arg): arg(_arg) {}
public:
    virtual const char * getFunction() const =0;

    ExpressionPtr getArg() const
    { return arg; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<getFunction()<<"( ";
        arg->print(dst);
        dst<<" )";
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        // NOTE : This should be implemented by the inheriting function nodes, e.g. LogFunction
        throw std::runtime_error("FunctionOperator::evaluate is not implemented.");
    }
};

class LogFunction
    : public Function
{
public:
    LogFunction(ExpressionPtr _arg)
        : Function(_arg)
    {}

    virtual const char *getFunction() const
    { return "log"; }
    
    // TODO-E : Override evaluate, and implement it

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override 
    {
        double l_arg = getArg()->evaluate(bindings);
        return log(l_arg);
    }


};

class ExpFunction
    : public Function
{
public:
    ExpFunction(ExpressionPtr _arg)
        : Function(_arg)
    {}

    virtual const char *getFunction() const
    { return "exp"; }

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override {
        double e_arg = getArg()->evaluate(bindings);
        return exp(e_arg);
    }

};

class SqrtFunction
    : public Function
{
public:
    SqrtFunction(ExpressionPtr _arg)
        : Function(_arg)
    {}

    virtual const char *getFunction() const
    { return "sqrt"; }

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override {
        double s_arg = getArg()->evaluate(bindings);
        return sqrt(s_arg);
    }
};


#endif
