#ifndef ast_loops_hpp
#define ast_loops_hpp

#include "ast.hpp"

class WhileStatement: public Expression {
private:
    ExpressionPtr Condition;
    NodePtr Body;

public:
    ~WhileStatement() {}

    WhileStatement (ExpressionPtr _Condition, NodePtr _Body) : Condition(_Condition) , Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        dst << "while(";
        Condition->print(dst);
        dst <<") ";
        Body->print(dst);
    }

    virtual void translate (std::ostream &dst) const override {
        dst<< "while(";
        Condition->translate(dst);
        dst<<"): ";
        Body->translate(dst);
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {
        loop_while = true;

        if(loop_while == true){
        
        //use a free register for condition check
 
        int current_loop = loop_count++;

        dst<<"while_loop_"<<current_loop<<"_begin:"<<"\t#Begin while loop"<<std::endl;
        
        //evalute the condition into the free register
        Condition->compile(dst, contxt, destReg);
        //branch to end if condition evaluates false (0)
        dst<<"\t"<<"beq"<<"\t"<<"$0, $"<<destReg<<", end_loop_"<<current_loop<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;

        std::string current_loop_end = "end_loop_" + std::to_string(current_loop);
        loop_ends.push_back(current_loop_end);

        Body->compile(dst, contxt, destReg);
        // loop_count--;


        //branch back to top
        dst<<"\t"<<"b"<<"\t"<<"while_loop_"<<current_loop<<"_begin"<<std::endl;
        dst<<"\t"<<"nop"<<std::endl;
        //end of loop
        dst<<current_loop_end<<":"<<"\t#End while loop"<<std::endl;
        loop_ends.pop_back();
        }

        loop_while = false;
    }
};

class DoWhileStatement: public Expression {
private:
    ExpressionPtr Condition;
    NodePtr Body;

public:
    ~DoWhileStatement() {}

    DoWhileStatement (ExpressionPtr _Condition, NodePtr _Body) : Condition(_Condition) , Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        dst << "do" <<std::endl;
        dst<<"\t";
        Body->print(dst);
        dst<<"while(";
        Condition->print(dst);
        dst <<");"<<std::endl;
        
    }
};

class ForStatement: public Expression {
private:
    NodePtr Init;
    ExpressionPtr Condition;
    NodePtr Increment;
    NodePtr Body;

public:
    ~ForStatement() {}

    ForStatement (NodePtr _Init, ExpressionPtr _Condition, NodePtr _Increment, NodePtr _Body)
     : Init(_Init), Condition(_Condition) , Increment(_Increment), Body(_Body) {}

    virtual void print (std::ostream &dst) const override {
        dst << "for(";
        Init->print(dst);
        Condition->print(dst);
        dst<<";";
        Increment->print(dst);
        dst<<")"<<std::endl;
        dst<<"\t";
        Body->print(dst);        
    }

    virtual void compile(std::ostream &dst, Context &contxt, int destReg) const override {

        loop_for = true;

        if(loop_for == true){
            //use a free register for condition check
            // std::vector<int> freeRegs = contxt.FindFreeConstantRegs();
            // contxt.set_used(freeRegs[0]);

            loop_for = true;
            if (Init != NULL)
                Init->compile(dst, contxt, destReg);
            dst<<"for_loop_"<<loop_count<<"_begin:"<<"\t#Begin for loop"<<std::endl;
            
            //evalute the condition into the free register
            if (Condition != NULL)
                Condition->compile(dst, contxt, destReg);
            //branch to end if condition evaluates false (0)
            dst<<"\t"<<"beq"<<"\t"<<"$0, $"<<destReg<<", end_loop_"<<loop_count<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;

            std::string current_loop_end = "end_loop_" + std::to_string(loop_count);
            loop_ends.push_back(current_loop_end);
            if (Body != NULL)
                Body->compile(dst, contxt, destReg);
            // loop_count--;

            dst<<"for_increment_"<<loop_count<<":"<<"\t#Increment stage of for loop"<<std::endl;
            if (Increment != NULL)
                Increment->compile(dst, contxt, destReg);

            //branch back to top
            dst<<"\t"<<"b"<<"\t"<<"for_loop_"<<loop_count<<"_begin"<<std::endl;
            dst<<"\t"<<"nop"<<std::endl;
            //end of loop
            dst<<"end_loop_"<<loop_count<<":"<<"\t#End for loop"<<std::endl;
            loop_ends.pop_back();
            // contxt.set_unused(freeRegs[0]);
            loop_count++;
        }

        loop_for = false;
    }
};

#endif