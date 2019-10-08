#ifndef ast_enum_hpp
#define ast_enum_hpp

#include "ast.hpp"

class EnumDeclaration : public Expression {
    private:
    std::string listname;
    ExpressionPtr varlist;

  public:
    ~EnumDeclaration () {}
    EnumDeclaration(std::string _listname, ExpressionPtr _varlist) : listname(_listname), varlist(_varlist) {}

    virtual void print (std::ostream &dst) const override {
        dst<<"enum "<<listname<<"{";
        varlist->print(dst);
        dst<<"};";
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        std::cout<<"#enum declaration"<<std::endl;
        enum_list_size = 0;
        std::ostream tmp(0);
        varlist->print(tmp);
        varlist->compile(dst, contxt, destReg);
        std::cout<<"#enum list size "<<enum_list_size<<std::endl;
    }
};

class LocalEnumElement: public Expression {
  private:
    std::string current_element;
    ExpressionPtr el_value;
    ExpressionPtr next_el;
    // bool value_assigned;

  public:
    ~LocalEnumElement() {}
    LocalEnumElement(std::string _current_element, ExpressionPtr _el_value, ExpressionPtr _next_el)
     : current_element(_current_element), el_value(_el_value), next_el(_next_el) {}

    virtual void print (std::ostream &dst) const override {
        // enum_list_size++;
        dst<<current_element;
        if (el_value!=NULL) {
            dst<<" = ";
            el_value->print(dst);
            
        }
        if (next_el != NULL) {
            dst<<", ";
            next_el->print(dst);
        }
    }

    virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
        localvar_counter++;
        contxt.NewLocalVar(current_element + std::to_string(scopelevel));
        
        if (el_value!=NULL) {
            enum_list_size = el_value->evaluate();
        }
        
        dst<<"\t"<<"li"<<"\t"<<"$"<<destReg<<", "<<enum_list_size;
         dst<<"\t#enum variable "<<current_element<<" assigned "<<enum_list_size<<std::endl;
        
        enum_list_size++;
        
        dst<<"\t"<<"sw"<<"\t"<<"$"<<destReg<<", "<<contxt.LookupVariable(current_element, scopelevel)<<"($fp)";
        dst<<"\t#Store enum var "<<current_element<<std::endl;
        if (next_el != NULL) {
            std::vector<int> freeRegs = contxt.FindFreeTempRegs();
            contxt.set_used(freeRegs[0]);
            next_el->compile(dst, contxt, freeRegs[0]);
            contxt.set_unused(freeRegs[0]);
        }
    }
};

class GlobalEnumElement: public Expression {
  private:
    std::string current_element;
    ExpressionPtr el_value;
    ExpressionPtr next_el;
    // bool value_assigned;

  public:
    ~GlobalEnumElement() {}
    GlobalEnumElement(std::string _current_element, ExpressionPtr _el_value, ExpressionPtr _next_el)
     : current_element(_current_element), el_value(_el_value), next_el(_next_el)  {}

    virtual void print (std::ostream &dst) const override {
        dst<<current_element;
        if (el_value != NULL) {
            dst<<" = ";
            el_value->print(dst);
            
        }
        if (next_el != NULL) {
            dst<<", ";
            next_el->print(dst);
        }
    }

        virtual void compile (std::ostream &dst, Context &contxt, int destReg) const override {
            dst << "\t"<< ".globl"<< "\t" << current_element <<"\t#New enum global variable"<< std::endl;
            dst << "\t"<< ".data"<< "\t" << std::endl;

            //store expression
            dst << current_element << ":" << std::endl;
            

            if (el_value != NULL) {
               enum_list_size = el_value->evaluate();
            }
            
            dst << "\t"<< ".word"<< "\t"<<enum_list_size;
            
            enum_list_size++;
            dst<<std::endl;
            contxt.NewGlobalVar(current_element);

            if (next_el != NULL) {
                next_el->compile(dst, contxt, destReg);
            }
        }

};

#endif