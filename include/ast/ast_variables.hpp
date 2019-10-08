#ifndef ast_variables_hpp
#define ast_variables_hpp

#include "ast.hpp"
#include <string>
#include <vector>

extern bool varGlobal;
// extern unsigned int localvar_counter;

class GlobalVarDec : public Expression
{
  public:
    std::string Type;
    std::string Name;
    ExpressionPtr Expression;

    GlobalVarDec(std::string _Type, std::string _Name, ExpressionPtr _Expression) : Type(_Type), Name(_Name), Expression(_Expression)
    {
        global_vars.push_back(Name);
        // var_count++;
    }
    ~GlobalVarDec() {}

    virtual void print(std::ostream &dst) const override
    {
        dst << Type << " " << Name;
        if (Expression != NULL)
        {
            dst << " = ";
            Expression->print(dst);
        }
        dst << ";" << std::endl;
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << Name;
        if (Expression != NULL)
        {
            dst << "=";
            Expression->translate(dst);
        }
        else
        {
            dst << "=0";
        }
        dst << std::endl;
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        dst << "\t"<< ".globl "<<Name << std::endl;
        dst << "\t"<< ".data" << std::endl;
        if (Expression != NULL)
        {
            // dst << "\t"<< ".align"<< "\t"<< "2" << std::endl;
            //dst<<"\t"<<".type"<<"\t"<<Name<<", @object"<<std::endl;
            //dst<<"\t"<<".size"<<"\t"<<Name<<", 4"<<std::endl;

            dst << Name << ":" << std::endl;
            //store expression
            dst << "\t"<< ".word"<< "\t";
            varGlobal = true;
            Expression->compile(dst, contxt, destReg);
            varGlobal = false;
            dst<< "\t#New global variable"<< std::endl;

            contxt.store_var_val(Name, Expression->evaluate());
            // dst << "\t"<< ".text" << std::endl;
            // dst << "\t"<< ".align"<< "\t"<< "2" << std::endl;
        }
        else {
            dst<<Name<<":"<<std::endl;
            dst<<"\t"<<".word"<<"\t"<<"0" << "\t#New global variable"<< std::endl;

        }

        contxt.NewGlobalVar(Name);
    }
};

class LocalVarDec : public Expression
{
  public:
    std::string Type;
    std::string Name;
    ExpressionPtr Expression;

    LocalVarDec(std::string _Type, std::string _Name, ExpressionPtr _Expression)
        : Type(_Type), Name(_Name), Expression(_Expression)
    {  
    }

    ~LocalVarDec() {}

    virtual void print(std::ostream &dst) const override
    {
        var_count++;
        dst << Type << " " << Name;
        if (Expression != NULL)
        {
            dst << " = ";
            Expression->print(dst);
        }
        dst << ";";
    }

    virtual void translate(std::ostream &dst) const override
    {
        dst << Name;
        if (Expression != NULL)
        {
            dst << " = ";
            Expression->translate(dst);
        }
        else
        {
            dst << "=0";
        }
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override
    {
        // dst << "#local variable" << std::endl;
        localvar_counter++;
        contxt.NewLocalVar(Name+std::to_string(scopelevel));
        if (Expression != NULL)
        {
            Expression->compile(dst, contxt, destReg);
            contxt.store_var_val(Name, Expression->evaluate());
        }
        else
        {
            dst << "\t"<<"li"<< "\t"<< "$"<<destReg<<", 0" << std::endl;
        }
        dst<<"#local var counter = "<<localvar_counter<<std::endl;
        dst << "\t"<<"sw"<< "\t"<< "$"<<destReg<<", "<<(localvar_counter-1)*4+16<< "($fp)";
        dst<<"\t#Storing variable "<<Name<< std::endl;
        // localvar_counter--;
    }
};

class MultipleDecs : public Expression
{
  private:
    std::string Type;
    ExpressionPtr AdditionalNames;
    bool isGlobal;

  protected:
  public:
    ~MultipleDecs() {}
    MultipleDecs(std::string _Type, std::string _Name, ExpressionPtr _AdditionalNames, bool _isGlobal)
        : Type(_Type), AdditionalNames(_AdditionalNames), isGlobal(_isGlobal)
    {
        if (isGlobal)
            GlobalNames.push_back(_Name);
        else
            LocalNames.push_back(_Name);
    }
    MultipleDecs() {} //for inheritance

    virtual void print(std::ostream &dst) const override
    {
        AdditionalNames->print(dst);
        //after recursive call
        if (isGlobal)
        {
            for (unsigned int i = 0; i < GlobalNames.size(); i++)
            {
                dst << Type << " " << GlobalNames[i];
                if (GlobalNameExpr[i] != NULL)
                {
                    dst << " = ";
                    GlobalNameExpr[i]->print(dst);
                }
                dst << ";" << std::endl;
                for (int i = 0; i < scopelevel; i++)
                {
                    dst << "\t";
                }
            }
        }
        else
        {
            for (unsigned int i = 0; i < LocalNames.size(); i++)
            {
                dst << Type << " " << LocalNames[i];
                if (LocalNameExpr[i] != NULL)
                {
                    dst << " = ";
                    LocalNameExpr[i]->print(dst);
                }
                dst << ";" << std::endl;
                for (int i = 0; i < scopelevel; i++)
                {
                    dst << "\t";
                }
            }
        }
    }

    virtual void translate(std::ostream &dst) const override
    {
        AdditionalNames->translate(dst);
        //after recursive call
        if (isGlobal)
        {
            for (unsigned int i = 0; i < GlobalNames.size(); i++)
            {
                dst << GlobalNames[i];
                if (GlobalNameExpr[i] != NULL)
                {
                    dst << " = ";
                    GlobalNameExpr[i]->translate(dst);
                }
                else
                {
                    dst << " = 0";
                }
                dst << std::endl;
                for (int i = 0; i < scopelevel; i++)
                {
                    dst << "\t";
                }
            }
        }
        else
        {
            for (unsigned int i = 0; i < LocalNames.size(); i++)
            {
                dst << LocalNames[i];
                if (LocalNameExpr[i] != NULL)
                {
                    dst << " = ";
                    LocalNameExpr[i]->translate(dst);
                }
                else
                {
                    dst << " = 0";
                }
                dst << std::endl;
                for (int i = 0; i < scopelevel; i++)
                {
                    dst << "\t";
                }
            }
        }
    }
};

class AdditionalDecs : public MultipleDecs
{
  private:
    std::string CurrentVar;
    ExpressionPtr CurrentExpression;
    ExpressionPtr NextVar;
    bool isGlobal;

  public:
    ~AdditionalDecs() {}

    AdditionalDecs(std::string _CurrentVar, ExpressionPtr _CurrentExpression, ExpressionPtr _NextVar, bool _isGlobal)
        : CurrentVar(_CurrentVar), CurrentExpression(_CurrentExpression), NextVar(_NextVar), isGlobal(_isGlobal)
    {
        if (isGlobal)
        {
            GlobalNames.push_back(CurrentVar);
            GlobalNameExpr.push_back(CurrentExpression);
        }

        else
        {
            LocalNames.push_back(CurrentVar);
            LocalNameExpr.push_back(CurrentExpression);
        }
    }

    virtual void print(std::ostream &dst) const override
    {
        if (NextVar != NULL)
        {
            NextVar->print(dst);
        }
    }

    virtual void translate(std::ostream &dst) const override
    {
        if (NextVar != NULL)
        {
            NextVar->translate(dst);
        }
    }
};

class GlobalArrayDec : public Expression {
private:
  std::string Type;
  std::string Identifier;
  int arraysize;
public:
  ~GlobalArrayDec() {}
  GlobalArrayDec(std::string _Type, std::string _Identifier, int _arraysize) : Type(_Type), Identifier(_Identifier), arraysize(_arraysize) {}

  virtual void print(std::ostream &dst) const override{
    //   var_count = var_count + arraysize;
        dst << Type << " " << Identifier << "[" << arraysize << "];";
  }

  virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        // dst<<"#global array"<<std::endl;
        for (int i=0;i<arraysize;i++) {
            // localvar_counter++;
            std::string arrayid = Identifier + std::to_string(i);
            contxt.NewGlobalVar(arrayid + std::to_string(scopelevel));
        }
        dst<<"\t"<<".data"<<std::endl;
        dst<<Identifier<<":\t"<<".space "<<arraysize*4<<"\t#Global array of size "<<arraysize<<std::endl;
        dst<<std::endl;

  }
};

class LocalArrayDec : public Expression 
{
private:
    std::string Type;
    std::string Identifier;
    int arraysize;
public:
    ~LocalArrayDec() {}
    LocalArrayDec (std::string _Type, std::string _Identifier, int _arraysize) : Type(_Type), Identifier(_Identifier), arraysize(_arraysize) {}

    virtual void print (std::ostream &dst) const override {
        var_count = var_count + arraysize;
        dst<<Type<<" "<<Identifier<<"["<<arraysize<<"];";
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        
        dst << "#local array" << std::endl;
        for (int i=0;i<arraysize;i++) {
            localvar_counter++;
            std::string arrayid = Identifier + std::to_string(i);
            contxt.NewLocalVar(arrayid + std::to_string(scopelevel));
        }
        
        // localvar_counter = localvar_counter - arraysize;
    }
};

#endif