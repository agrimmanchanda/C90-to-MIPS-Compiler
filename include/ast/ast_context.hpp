#ifndef ast_context_hpp
#define ast_context_hpp

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

static unsigned int localvar_counter;
static unsigned int globalvar_counter;
class Context {
    public:
    //track local and global variables
    //string is name of variable
    //int is relative location in memory
    std::unordered_map<std::string, unsigned int> globalbindings;
    std::unordered_map<std::string,unsigned int> localbindings;
    std::unordered_map<std::string, unsigned int> var_vals;
    std::vector<std::string>parameter_list;

    bool regavailable[32];
    

    ~Context() {}

    Context() {
        //initially set registers to all available except:
        //0:zero reg & 1
        //2 & 3: return reg
        //26 & 27: kernel reg
        //28: global pointer
        //29: stack pointer
        //30: frame pointer
        //31: return address of subroutine
        for (int i = 0;i < 32;i++) {
            if (i <= 3 || i >= 26)
                regavailable[i] = false;
            else
                regavailable[i] = true;
        }

        localvar_counter = 0;
        globalvar_counter = 0;
    }

    void set_used(unsigned int regno) {
        std::cout<<"#locked: "<<regno<<std::endl;
        // std::cout<<"#: "<<regavailable[regno]<<std::endl;
        regavailable[regno] = false;
    }

    void set_unused(unsigned int regno) {
        std::cout << "#unlocked: " << regno << std::endl;
        // std::cout << "#: " << regavailable[regno] << std::endl;
        regavailable[regno] = true;
    }

    std::vector<int> FindFreeRegs (unsigned int first, unsigned int last) {
        //finds and returns register numbers that are free for the defined range
        std::vector<int> FreeRegs;
        for (unsigned int i = first; i <= last; i++) {
            if (regavailable[i]) {
                FreeRegs.push_back(i);
            }
        } 
        if (FreeRegs.size() == 0) {
            throw std::runtime_error ("No free temp regs");
        } 
        return FreeRegs;     
    }

    std::vector<int> FindLockedTempRegs () {
        //finds and returns tempreg nos that are locked
        std::vector<int> LockedRegs;
        for (unsigned int i = 8; i <= 15; i++) {
            if (!regavailable[i]) {
                LockedRegs.push_back(i);
            }
        }
        for (unsigned int i = 24; i <= 25; i++) {
            if (!regavailable[i]) {
                LockedRegs.push_back(i);
            }
        }    
        return LockedRegs;     
    }

    std::vector<int> FindFreeTempRegs () {
        std::vector<int> FreeTemp;
        std::vector<int> set1 = FindFreeRegs(8,15);
        std::vector<int> set2 = FindFreeRegs(24,25);
        FreeTemp.reserve(set1.size() + set2.size());
        FreeTemp.insert(FreeTemp.end(), set1.begin(), set1.end());
        FreeTemp.insert(FreeTemp.end(), set2.begin(), set2.end());
        // std::cout<<"#registers:" << std::endl;
        // for (int  i=0;i<32;i++) {
        //     std::cout<<"#"<<i<<": "<<regavailable[i]<<std::endl;
        // }
        // std::cout<<"#End list"<<std::endl;
        return FreeTemp;
    }

    std::vector<int> FindFreeConstantRegs () {
        return FindFreeRegs(16,23);

    }
    std::vector<int> FindFreeParamRegs () {
        return FindFreeRegs(4,7);
    }


    void NewGlobalVar (std::string varname) {
        globalbindings.emplace(varname,++globalvar_counter);

    }

    void DeleteLocalVar (std::string varname) {
        localbindings.erase(varname);
    }

    void NewLocalVar (std::string varname) {
        //assign a new number to keep track of the variable dec
        localbindings.emplace(varname,localvar_counter);
    }

    int LookupVariable (std::string varname, int scopelevel) {
        for (int i = scopelevel; i>=0; i--) {
            std::string search_str = varname + std::to_string(scopelevel);
            //scopelevel added as variable may be redeclared in new scope
            if (localbindings.find(search_str) != localbindings.end()) {
                //variable found
                //return the memory location of the variable
                return ((localbindings.find(search_str)->second-1)*4+16);
            }
        }
        //if not found return 0
        return 0;
    }

    void clearArgumentregs() {
        for (int i=4; i<8;i++) {
            regavailable[i] = true;
        }
    }

    void set_parameter_list(std::string parameter_name){
        if(parameter_list.size()>4){
            throw std::runtime_error("Too many parameters"); 
        }
        parameter_list.push_back(parameter_name);
    }

    int find_parameter(std::string parameter_name){
        for(unsigned int i =0; i<parameter_list.size(); i++){
            if(parameter_name == parameter_list[i]){
                return i;
            }
        }

        return -1;
    }

    void clear_parameter_list(){
        parameter_list.clear();
    }

    void store_var_val(std::string var, int val) {
        var_vals.erase(var);
        var_vals.emplace(var, val);
    }

    void clear_var_val(std::string var) {
        var_vals.erase(var);
    }

    int find_var_val(std::string var) {
        if (var_vals.find(var) != var_vals.end())
        {
            //variable found
            return (var_vals.find(var)->second);
        }
        return 0;
    }


};

#endif