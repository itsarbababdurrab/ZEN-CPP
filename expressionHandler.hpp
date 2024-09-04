#ifndef EXPRESSIONHANDLER_HPP
#define EXPRESSIONHANDLER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include "main.hpp"
using namespace std;

class ExpressionHandler{
    public:
        bool isInteger(double value);
        bool isBool(double value);
        MainClass::dataTypes_variant exp_handler(vector<string> tokens, int line_num, const string& exp);
};

#endif 