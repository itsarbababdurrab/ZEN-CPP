#ifndef SET_HPP
#define SET_HPP

#include <iostream>
#include <string>
#include <variant>
#include <vector>
using namespace std;

class Set{
    public:
        bool isInteger(double value);
        void printValue(const string& message, const variant<int, float, double, string, bool>& value);
        void set_user(vector<string> tokens, int line_num, string message, string variable, string exp);
};


#endif 
