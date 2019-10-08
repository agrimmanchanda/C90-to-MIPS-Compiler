#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>
extern bool varGlobal;

class Variable: public Expression {
    private:
    std::string id;
    public:
    Variable(const std::string &_id)
        : id(_id)
    {}

    const std::string getId() const
    { return id; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<id;
    }

    virtual void translate(std::ostream &dst) const {
        dst<<id;
    }


    virtual int evaluate() const override
    {
        // std::cout<<"#variable"<<std::endl;
        expr_var = id;
        return 0;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        int var_offset = contxt.LookupVariable(id, scopelevel);
        // std::cout<<"VAR OFFSET: "<<var_offset<<std::endl;

        int stack_end = (var_count*4) +parameter_count+12+50; 

        std::vector<int> freeArgReg = contxt.FindFreeRegs(4, 7);

        if(var_offset != 0){  //if (var_offset == 0)
            //looks for local variables in the scopes
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<var_offset<<"($fp)"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
        }
        else if(contxt.find_parameter(id) > -1){
            // for(int i =0; i<4; i++){
            //     contxt.set_used(destReg+i);
            //     dst<<"\t"<<"lw"<<"\t"<<"$"<<(destReg+i)<<", "<<stack_end+(4*i)<<"($fp)"<<"\t"<<"#loading param argument register"<<std::endl;
            // }
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<(contxt.find_parameter(id))*4+stack_end<<"($fp)";
            dst<<"\t #found variable "<<id<<" in parameters"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
        }
        else{
            //loads global variables
            dst<<"\t"<<"lui"<<"\t"<<"$"<<destReg<<", "<<"%hi("<<id<<")"<<"\t#Loading in global variable: "<<id<<std::endl;
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<"%lo("<<id<<")($"<<destReg<<")"<<std::endl;
            dst<<"\t"<<"nop"<<"\t#Global variable load nop"<<std::endl;
        }
    }   
};

class ArrayElement : public Expression {
    private:
    std::string id;
    ExpressionPtr element;

    public:
    ~ArrayElement() {};
    ArrayElement(std::string _id, ExpressionPtr _element) : id(_id), element(_element) {}
    virtual void print (std::ostream &dst) const override {
        dst<<id<<"[";
        if (element!=NULL)
            element->print(dst);
        dst<<"]";
    }

    virtual int evaluate () const override {
        return element->evaluate();
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override{
        int element_no = element->evaluate();
        if (expr_var != "") {
            if (element_no == 1)
                element_no = contxt.find_var_val(expr_var) - 1;
            else if (element_no == 0)
                element_no = contxt.find_var_val(expr_var);
            expr_var = "";
        }
        if (element_no == 0) {
            element_no = element->evaluate();
        }
        int var_offset = contxt.LookupVariable(id+std::to_string(element_no), scopelevel);
        // int stack_end = (var_count*4) +parameter_count+12+50;

        std::vector<int> freeArgReg = contxt.FindFreeParamRegs();

        if(var_offset != 0) {  
            // dst<<"#var offset: "<<var_offset<<std::endl;
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<var_offset<<"($fp)";
            dst<<"\t#Accessing element "<<element_no<<" of array "<<id<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
        }
        else {
            //global array
            dst<<"\t"<<"lui"<<"\t"<<"$"<<destReg<<", "<<"%hi("<<id<<")"<<"\t#Loading in array: "<<id<<std::endl;
            dst<<"\t"<<"addiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", %lo("<<id<<")"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
            dst<<"\t"<<"addiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", "<<element_no*4<<"\t#Add offset for element "<<element_no<<std::endl;
            dst<<"\t"<<"lw"<<"\t"<<"$"<<destReg<<", "<<"0($"<<destReg<<")"<<"\t#Load in element "<<element_no<<" of global array "<<id<<std::endl;
        }
    }
};

class Number: public Expression
{
private:
    double value;
public:
    Number(double _value)
        : value(_value)
    {}

    double getValue() const
    { return value; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<value;
    }

    virtual void translate(std::ostream &dst) const {
        dst<<value;
    }


    // virtual double evaluate(
    //     const std::map<std::string,double> &bindings
    // ) const override
    // {
    //     // TODO-A : Run bin/eval_expr with a numeric expression to make sure you understand how this works.
    //     return value;
    // }
    virtual int evaluate () const override {
        return value;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        if (varGlobal) {
            dst<<value<<"\t#Global variable"<<std::endl;
        }
        else if (!varGlobal) {
            dst <<"\t"<< "li" << "\t" << "$"<<destReg<<", " << value<<std::endl;
        }
	}
};


#endif
