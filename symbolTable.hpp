#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP


#include <iostream>
#include <vector>
#include <variant>
#include <unordered_map>
#include "main.hpp"
using namespace std;

class SymbolTable{
    public:
        struct Symbols{
            string dataType;
            string name;
            MainClass::dataTypes_variant value;
            string type;
        };
        unordered_map<string, Symbols> stm_dict; 

        void create(const vector<MainClass::dataTypes_variant>& tokens, int line_num);
        MainClass::dataTypes_variant retrieve(const vector<string>& tokens, const string& variable, const string& info_need, int line_num);

};


#endif 
