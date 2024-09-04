#ifndef SYNTAXANALYZER_HPP
#define SYNTAXANALYZER_HPP

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class SyntaxAnalyzer{
    public:
        void parser(vector<string>& tokens, int line_num, vector<string>& dataTypes, vector<string>& grammarSymbols);
};



#endif 
