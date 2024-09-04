#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <vector>
#include <variant>
using namespace std;

class MainClass{
    public:
        // Basic components of the compiler
        using dataTypes_variant = variant<int, float, string, bool>;
        string variable_regex = "^[a-zA-Z_][a-zA-Z\\d_]*$";
        string int_float_regex = "(^\\d+$)|(^\\d*[\\.]\\d+$)";
        string int_regex = "^\\d+$";
        string float_regex = "^\\d*[\\.]\\d+$";
        string trim(const string& str){
            // cout<<"string: "<<str<<endl;
            size_t first = str.find_first_not_of(' ');
            size_t last = str.find_last_not_of(' ');
            if(first == string::npos || last == string::npos){
                return "";
            }
            return str.substr(first, last - first + 1);
        }
        string concacte(const vector<string> tokens, size_t start, size_t end){
            string joined;
            for(size_t i = start; i<end; ++i){
                if(!joined.empty()){
                    joined+= " "; // add space after a word
                }
                joined+=tokens[i];
            }
            return joined;
        }
        // define datatypes
        vector<string> dataTypes= {
            "Num",      // integer
            "Literal",  // string
            "Decimal",  // float
            "Bool"      // boolean
        };
        // define grammar symbols
        vector<string> grammarSymbols = {
            "Set",
            "Get"
        };
        void readFile(string& file_name);

};


#endif
