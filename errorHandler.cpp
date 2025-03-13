
#include <iostream>
#include <string>
#include <vector>
#include "errorHandler.hpp"
using namespace std;

void ErrorHandler::handleError(int line_num, string desc, string type, vector<string> content){
    if(type == "Syntax error"){
        throw Error(type, desc , line_num, content);
    }else if(type == "Value Error"){
        throw Error(type, desc, line_num, content);
    }
}

