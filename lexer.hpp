#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <variant>
#include "main.hpp"
using namespace std;

class Lexer{
    public:
        void lexicalAnalyzer(string& content, int line_num, vector<string>& dataTypes, vector<string>& grammarSymbols);
        vector<string> tokenize(const string& content);
        void prnt(const MainClass::dataTypes_variant& var);
        
};



#endif 
