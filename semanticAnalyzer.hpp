#ifndef SEMANTICANALYZER_HPP
#define SEMANTICANALYZER_HPP

#include <iostream>
#include <string>
#include <vector>
#include "main.hpp"
using namespace std;



class SemanticAnalyzer{
    public:
        
        bool startswith(const string& str, char c);
        bool endswith(const string& str, char c);
        string concacteTokens(const vector<MainClass::dataTypes_variant> tokens, size_t start, size_t end);
        void semantic_analyzer(vector<string>& tkns, int line_num);
};



#endif 
