#ifndef ast_operators_hpp
#define ast_operators_hpp

#include <string>
#include <iostream>
#include <cmath>

class Operator
    : public Expression
{
protected:
    ExpressionPtr left;
    ExpressionPtr right;

    Operator(ExpressionPtr _left, ExpressionPtr _right)
        : left(_left)
        , right(_right)
    {}
public:
    virtual const char *getOpcode() const =0;

    ExpressionPtr getLeft() const
    { return left; }

    ExpressionPtr getRight() const
    { return right; }

    virtual void print(std::ostream &dst) const override
    {
        // dst<<"( ";
        if (left != NULL)
            left->print(dst);
        
        dst<<getOpcode();
        
        if (right != NULL)
            right->print(dst);
    //    dst<<" )";
    }

    virtual void translate(std::ostream &dst) const {
        if (left != NULL)
            left->translate(dst);
        dst << " ";
        dst << getOpcode();
        dst << " ";
        if (right != NULL)
            right->translate(dst);
    }

};

    class AddOperator
        : public Operator
    {
      protected:
        virtual const char *getOpcode() const override
        {
            return "+";
        }

      public:
        AddOperator(ExpressionPtr _left, ExpressionPtr _right)
            : Operator(_left, _right)
        { }

        virtual int evaluate () const override
        {
            int vl = left->evaluate();
            int vr = right->evaluate();
            return vl + vr;
        }

        virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
        {
            if (varGlobal)
            {
                int vl = left->evaluate();
                int vr = right->evaluate();
                dst << vl + vr;
            }
            else
            {
                std::vector<int> freeregs = contxt.FindFreeTempRegs(); //finds available registers
                contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
                
                int current_function_depth = function_call_num;
                std::cout<<"#function call depth: "<<current_function_depth<<std::endl;
                left->compile(dst, contxt, freeregs[0]);
                if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[0]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }
                current_function_depth = function_call_num;

                right->compile(dst, contxt, freeregs[1]);
                if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[1]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }
                dst << "\t"<< "addu"<< "\t"<< "$" << destReg << ", $" << freeregs[0] << ", $" << freeregs[1] << "\t#Add operator" << std::endl;
                contxt.set_unused(freeregs[0]);
                contxt.set_unused(freeregs[1]);
            }
    }

};

class SubOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "-"; }
public:
    SubOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}
    
    virtual int evaluate() const override
    {
        int vl = left->evaluate();
        int vr = right->evaluate();
        return vl - vr;
        
    }
    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        if (varGlobal) {
            int vl = left->evaluate();
            int vr = right->evaluate();
            dst<<vl - vr;
        }
        else {
            std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
            contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
            
            left->compile(dst, contxt, destReg);
            right->compile(dst, contxt, freeReg[0]);
            dst << "\t"<< "sub"<< "\t"<< "$" << destReg << ", $" << destReg << ", $" << freeReg[0] <<"\t#Sub Operator"<< std::endl;
            contxt.set_unused(freeReg[0]);
        }
    }
};


class MulOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "*"; }
public:
    MulOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate () const override
    {
        int vl = left->evaluate();
        int vr = right->evaluate();
        return vl * vr;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        if (varGlobal) {
            int vl = left->evaluate();
            int vr = right->evaluate();
            dst<<vl * vr;
        }
        else {
            std::vector<int> freeregs = contxt.FindFreeTempRegs(); //finds available registers
            contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
            contxt.set_used(freeregs[1]);
            int current_function_depth = function_call_num;
            std::cout << "#function call depth: " << current_function_depth << std::endl;

            left->compile(dst, contxt, freeregs[0]);
            if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[0]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }
            current_function_depth = function_call_num;

            right->compile(dst, contxt, destReg);
            if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[1]<<", $"<<destReg<<"\t#Function call result"<<std::endl;
                }

            dst << "\t"<<"mult"<<"\t"<< "$" << freeregs[0] << ", $" << destReg <<"\t#Multiply Operator"<< std::endl;
            dst << "\t"<<"mflo"<<"\t"<<"$"<<destReg<<"\t#Store result of multiply"<<std::endl;
            contxt.set_unused(freeregs[0]);
            contxt.set_unused(freeregs[1]);

        }
    }
};

class DivOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "/"; }
public:
    DivOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        //throw std::runtime_error("DivOperator::evaluate is not implemented.");

        double vl=left->evaluate();
        double vr=right->evaluate();
        return vl/vr;
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        if (varGlobal) {
            int vl = left->evaluate();
            int vr = right->evaluate();
            dst<<vl / vr;
        }
        else {
            std::vector<int> freeregs = contxt.FindFreeTempRegs(); //finds available registers
            contxt.set_used(freeregs[0]);                      //locks the registers for use of the function
            int current_function_depth = function_call_num;
            std::cout << "#function call depth: " << current_function_depth << std::endl;

            left->compile(dst, contxt, freeregs[0]);
            if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[0]<<", $2"<<"\t#Function call result"<<std::endl;
                }
            current_function_depth = function_call_num;

            right->compile(dst, contxt, destReg);
            if (current_function_depth != function_call_num) {
                    dst<<"\t"<<"move"<<"\t"<<"$"<<freeregs[1]<<", $2"<<"\t#Function call result"<<std::endl;
            }

            dst << "\t"<<"div"<<"\t"<< "$" << freeregs[0] << ", $" << destReg <<"\t#Division Operator"<< std::endl;
            dst << "\t"<<"mflo"<<"\t"<<"$"<<destReg<<"\t#Store result"<<std::endl;
            contxt.set_unused(freeregs[0]);

        }
    }
};

class ModOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "%"; }
public:
    ModOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=left->evaluate();
        int vr=right->evaluate();
        return vl%vr;
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        if (varGlobal) {
            int vl = left->evaluate();
            int vr = right->evaluate();
            dst<<vl % vr;
        }
        else {
            std::vector<int> freeregs = contxt.FindFreeTempRegs(); //finds available registers
            contxt.set_used(freeregs[0]);                      //locks the registers for use of the function

            left->compile(dst, contxt, freeregs[0]);
  
            right->compile(dst, contxt, destReg);
          
            dst << "\t"<<"div"<<"\t"<< "$" << freeregs[0] << ", $" << destReg <<"\t#Mod Operator"<< std::endl;
            dst << "\t"<<"mfhi"<<"\t"<<"$"<<destReg<<"\t#Store result of mod operator"<<std::endl;
            contxt.set_unused(freeregs[0]);

        }
    }
};

// class ExpOperator
//     : public Operator
// {
// protected:
//     virtual const char *getOpcode() const override
//     { return "^"; }
// public:
//     ExpOperator(ExpressionPtr _left, ExpressionPtr _right)
//         : Operator(_left, _right)
//     {}

//     virtual double evaluate(
//         const std::map<std::string,double> &bindings
//     ) const override
//     {
//         //throw std::runtime_error("ExpOperator::evaluate is not implemented.");

//         double vl=left->evaluate(bindings);
//         double vr=right->evaluate(bindings);
//         return pow(vl, vr);
//     }
// };

class LessThanOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "<"; }
public:
    LessThanOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate () const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl<vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
        dst << "\t"<< "slt"<< "\t"<< "$" << destReg << ", $" << destReg << ", $" << freeReg[0] << "\t#< operator" << std::endl;
        contxt.set_unused(freeReg[0]);
    }
};

class LessThanEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "<="; }
public:
    LessThanEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl<=vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
        dst << "\t"<< "slt"<< "\t"<< "$" << destReg << ", $" << freeReg[0] << ", $" <<destReg<< "\t#<= operator" << std::endl;
        dst<<"\t"<<"xori"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", 1"<<"\t#<= operator" << std::endl;
        contxt.set_unused(freeReg[0]);
    }
};


class GreaterThanOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return ">"; }
public:
    GreaterThanOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl>vr);
    }
    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
        dst << "\t"<< "slt"<< "\t"<< "$" << destReg << ", $" << freeReg[0] << ", $" << destReg << "\t#> operator" << std::endl;
        contxt.set_unused(freeReg[0]);
    }
};

class GreaterThanEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return ">="; }
public:
    GreaterThanEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl>=vr);
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
        dst<<"\t"<< "slt"<<"\t"<< "$" << destReg << ", $" << destReg << ", $" << freeReg[0] << "\t#>= operator" << std::endl;
        dst<<"\t"<<"xori"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", 1"<<"\t#>= operator" << std::endl;
        contxt.set_unused(freeReg[0]);
    }
};

class IsEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "=="; }
public:
    IsEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl==vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"xor"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t#== operator" << std::endl;
        //hence if if zero, set destreg to 1 (since they are equal) but 0 otherwise
        dst<<"\t"<<"sltiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", 1"<<std::endl;
        contxt.set_unused(freeReg[0]);
    }
};

class IsNotEqualOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "!="; }
public:
    IsNotEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl!=vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"xor"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t#!= operator" << std::endl;
        //esnures that if the result is non zero it becomes 1, hence compare with zero register (a non zero will be greater)
        dst<<"\t"<<"sltu"<<"\t"<<"$"<<destReg<<", $0"<<", $"<<destReg<<std::endl;
        contxt.set_unused(freeReg[0]);
    }
};

class NotOperator : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return "!"; }

  public:
    NotOperator(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        // double vl=left->evaluate(bindings);
        int vr=right->evaluate();
        return (!vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        right->compile(dst, contxt, destReg);
        dst<<"\t"<<"sltiu"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", 1"<<"\t#! logical operator" << std::endl;
    }
};

class NegativeOperator : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return "-"; }

  public:
    NegativeOperator(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        int vr=right->evaluate();
        return (-vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        if (varGlobal) {
            int vr = right->evaluate();
            dst << -vr;
        }
        right->compile(dst, contxt, destReg);
        dst<<"\t"<<"sub"<<"\t"<<"$"<<destReg<<", $zero, $"<<destReg<<"\t#- operator" << std::endl;
    }
};


class LogicalAndOperator
    : public Operator
{
  protected:
    virtual const char *getOpcode() const override
    { return "&&"; }
  public:
    LogicalAndOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl&&vr);
    }

    virtual void translate (std::ostream &dst) const override {
        left->translate(dst);
        dst << " and ";
        right->translate(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"and"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t#&& logical operator" << std::endl;
        //and operation ensures that if the result is non zero it becomes 1
        contxt.set_unused(freeReg[0]);
    }
};

class LogicalOrOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "||"; }
public:
    LogicalOrOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        double vl=left->evaluate();
        double vr=right->evaluate();
        return (vl||vr);
    }

    virtual void translate (std::ostream &dst) const override {
        left->translate(dst);
        dst << " or ";
        right->translate(dst);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"or"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t#&& logical operator" << std::endl;
        //or operation ensures that if the result is non zero it becomes 1
        contxt.set_unused(freeReg[0]);
    }
};

class BitwiseComplement : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return "~"; }

  public:
    BitwiseComplement(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        // double vl=left->evaluate(bindings);
        int vr=right->evaluate();
        return (~vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        right->compile(dst, contxt, destReg);
        dst<<"\t"<<"nor"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $zero"<<"\t#~ operator" << std::endl;
    }  
};

class BitwiseOrOperator : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return "|"; }

  public:
    BitwiseOrOperator(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        int vl=left->evaluate();
        int vr=right->evaluate();
        return (vl|vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"or"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t#| bitwise operator" << std::endl;
        //or operation ensures that if the result is non zero it becomes 1
        contxt.set_unused(freeReg[0]);
    }   
};

class BitwiseXorOperator : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return "^"; }

  public:
    BitwiseXorOperator(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        int vl=left->evaluate();
        int vr=right->evaluate();
        return (vl^vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"xor"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t#^ bitwise operator" << std::endl;
        //or operation ensures that if the result is non zero it becomes 1
        contxt.set_unused(freeReg[0]);
    }   
};

class BitwiseAndOperator : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return "&"; }

  public:
    BitwiseAndOperator(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        int vl=left->evaluate();
        int vr=right->evaluate();
        return (vl|vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"and"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t#& bitwise operator" << std::endl;
        //or operation ensures that if the result is non zero it becomes 1
        contxt.set_unused(freeReg[0]);
    }   
};

class LeftShiftOperator : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return "<<"; }

  public:
    LeftShiftOperator(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        int vl=left->evaluate();
        int vr=right->evaluate();
        return (vl<<vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"sllv"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t# << operator" << std::endl;
        //or operation ensures that if the result is non zero it becomes 1
        contxt.set_unused(freeReg[0]);
    }   
};

class RightShiftOperator : public Operator {
  protected:
    virtual const char *getOpcode() const override
    { return ">>"; }

  public:
    RightShiftOperator(ExpressionPtr _left, ExpressionPtr _right) : Operator(_left, _right)
    {}

    virtual int evaluate() const override {
        int vl=left->evaluate();
        int vr=right->evaluate();
        return (vl>>vr);
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::vector<int> freeReg = contxt.FindFreeTempRegs(); //finds available registers
        contxt.set_used(freeReg[0]);                      //locks the registers for use of the function
        left->compile(dst, contxt, destReg);
        right->compile(dst, contxt, freeReg[0]);
     
        //checks equivalence, if they are the same will result in zero
        dst<<"\t"<<"srlv"<<"\t"<<"$"<<destReg<<", $"<<destReg<<", $"<<freeReg[0]<<"\t# << operator" << std::endl;
        //or operation ensures that if the result is non zero it becomes 1
        contxt.set_unused(freeReg[0]);
    }   
};

class TernaryOperator : public Expression {
  private:
    ExpressionPtr condition;
    ExpressionPtr con_true;
    ExpressionPtr con_false;
  public:
    ~TernaryOperator() {}
    TernaryOperator(ExpressionPtr _condition, ExpressionPtr _con_true, ExpressionPtr _con_false)
     : condition(_condition), con_true(_con_true), con_false(_con_false) {}
    
    virtual void print(std::ostream &dst) const override {
        condition->print(dst);
        dst<<" ? ";
        if (con_true != NULL) 
            con_true->print(dst);
        dst<<" : ";
        con_false->print(dst);
    }

    virtual int evaluate () const override {
        int con = condition->evaluate();
        int evaltrue; 
        if (con_true != NULL) {
            evaltrue = con_true->evaluate();
        }
        int evalfalse = con_false->evaluate();

        return con ? evaltrue : evalfalse;
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        int current_if_level = if_level++;
        condition->compile(dst, contxt, destReg);
        dst<<"\t"<<"#Ternary operator"<<std::endl;
        dst << "\t"<<"beq"<<"\t" << "$0, $" << destReg << ", else_"<<current_if_level << std::endl; //$else condition yet to be filled
		dst << "\t"<<"nop"<<"\t" << std::endl;

        if (con_true != NULL) {
            con_true->compile(dst, contxt, destReg);
        }
        else {
            //automatically jump to false expr since no true
            dst<<"\t"<<"b"<<"\t"<<"else_"<<current_if_level;
            dst<<"\t#No true expression provided"<<std::endl;
        }
        dst<<"\t"<<"b"<<"\t"<<"ifelse_end_"<<current_if_level<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;

        dst<<"else_"<<current_if_level<<":"<<std::endl;
        con_false->compile(dst, contxt, destReg);
    

        dst<<"ifelse_end_"<<current_if_level<<":"<<std::endl;
    }
};

#endif
