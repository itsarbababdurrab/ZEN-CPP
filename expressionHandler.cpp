#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cmath>
#include <variant>
#include "main.hpp"
#include "lexer.hpp"
#include "expressionHandler.hpp"
#include "symbolTable.hpp"
#include "errorHandler.hpp"
#include "tinyexpr.h"
#include <iomanip>
using namespace std;


bool ExpressionHandler::isInteger(double value){
    // cout<<typeid(value).name()<<" "<<value<<endl;
    return floor(value) == value;
}
bool ExpressionHandler::isBool(double value){
    return (value == 0 || value == 1);
}

MainClass::dataTypes_variant ExpressionHandler::exp_handler(vector<string> tokens, int line_num, const string& exp){
    MainClass mc;
    SymbolTable st;
    ErrorHandler err_h;
    te_parser tep;

    double result = 0.0;
    // cout<<exp<<endl;
    /* 
        handles two type of expressions
            1. has -> + - / * => for Num, Decimal (it will give us a value)
            2. has -> + - / * and & | => for Bool( it will only give us true or false)
    */
    string expression = "";
    string temp_exp = "";
    // cout<<"recieved by exp handler:"<<exp<<endl;
    if(regex_search(exp, regex("[&|]"))){
        // cout<<"recieved exp "<<exp<<endl;
        // for bool
        for(char ch: exp){
            if(ch == '&' || ch == '|'){
                if(temp_exp == "true" || temp_exp == "false"){
                    expression += (temp_exp == "true") ? "true" : "false";
                    temp_exp.clear();
                    expression+=ch;
                }else{
                    expression+=ch;
                }
            }else{
                temp_exp+=ch;   
            }
        }
        if(!temp_exp.empty()){
            if(temp_exp == "true" || temp_exp == "false"){
                expression += (temp_exp == "true") ? "true" : "false";
                temp_exp.clear();
            }else{
                // throw error /////////////////////////////////////////////////////////
                err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
            }
        }
        if(!expression.empty()){
            double res = tep.evaluate(expression);
            if(tep.success()){
                result = res;
            }else{
                // cout<<"evalaution failed"<<endl;
                err_h.handleError(line_num, "Unexpected Error", "Value Error", tokens);
            }                   
            
        }

    }else if(regex_search(exp, regex("[!<>=]"))){
        // for bool
        for(char ch: exp){
            if(string("!<>=").find(ch) != string::npos){
                if(regex_match(temp_exp, regex(mc.variable_regex))){
                    // cout<<temp_exp<<endl;
                    auto value = st.retrieve(tokens, temp_exp, "value", line_num);
                    temp_exp.clear();
                    if(holds_alternative<int>(value)){
                        int final_val = get<int>(value);
                        expression+=to_string(final_val);
                    }else if(holds_alternative<float>(value)){
                        float final_val = get<float>(value);
                        expression+=to_string(final_val);
                    }else{
                        // throw error
                        err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                    }
                    expression+=ch;
                }else{
                    expression+=temp_exp;
                    temp_exp.clear();
                    expression+=ch;
                }
            }else if(ch == '(' || ch == ')'){

            }else if(ch == ' '){

            }else{
                temp_exp+=ch;
            }
        }
        if(!temp_exp.empty()){
            if(regex_match(temp_exp, regex(mc.variable_regex))){
                auto value = st.retrieve(tokens, temp_exp, "value", line_num);
                temp_exp.clear();
                if(holds_alternative<int>(value)){
                    int final_val = get<int>(value);
                    expression+=to_string(final_val);
                }else if(holds_alternative<float>(value)){
                    float final_val = get<float>(value);
                    expression+=to_string(final_val);
                }else{
                    // throw error
                    err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                }

            }else{
                expression+=temp_exp;
                temp_exp.clear();
            }
        }
        /*
            evaluate the expression
            we will use tinyexpr.h library for expression evaluation
        */ 
        
        if(!expression.empty()){
            double res = tep.evaluate(expression);
            if(tep.success()){
                // result = tep.get_result();
                result = res;
                // cout<<typeid(result).name()<<endl;
            }else{
                // cout<<"evalaution failed"<<endl;
                err_h.handleError(line_num, "Unexpected Error", "Value Error", tokens);
            }    
        }
    }else{
        // for expression i.e, values
        for(char ch: exp){
            if(string("+-/*").find(ch) != string::npos){
                if(regex_search(temp_exp, regex("^[\\d\\.]"))){
                    expression+=temp_exp;
                    expression+=ch;
                    temp_exp.clear();
                }else if(regex_match(temp_exp, regex(mc.variable_regex))){
                    auto value = st.retrieve(tokens, temp_exp, "value", line_num);
                    temp_exp.clear();
                    if(holds_alternative<int>(value)){
                        int final_val = get<int>(value);
                        expression+=to_string(final_val);
                    }else if(holds_alternative<float>(value)){
                        float final_val = get<float>(value);
                        expression+=to_string(final_val);
                    }else{
                        // throw error
                        err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                    }
                    expression+=ch;
                }
            }else if(ch == '(' || ch == ')'){

            }else if(ch == ' '){

            }else{
                temp_exp+=ch;
            }
        }

        if(!temp_exp.empty()){
            // cout<<temp_exp<<endl;
            if(regex_search(temp_exp, regex("^[\\d\\.]"))){
                expression+=temp_exp;
                temp_exp.clear();
            }else if(regex_match(temp_exp, regex(mc.variable_regex))){
                // retrieve value from symbol table
                auto value = st.retrieve(tokens, temp_exp, "value", line_num);
                temp_exp.clear();
                if(holds_alternative<int>(value)){
                    int final_val = get<int>(value);
                    expression+=to_string(final_val);
                }else if(holds_alternative<float>(value)){
                    float final_val = get<float>(value);
                    expression+=to_string(final_val);
                }else {
                    // throw error
                    err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                }
            }
        }
        if(!expression.empty()){
            // cout<<"exp: "<<expression<<endl;
            if(regex_match(expression, regex("^[\\d\\.]+[+-\\/*=<>!]*[\\d]+"))){
                double res = tep.evaluate(expression);
                if(tep.success()){
                    result = res;
                }else{
                    err_h.handleError(line_num, "Unexpected Error", "Value Error", tokens);
                }    
            }else{
                err_h.handleError(line_num, "Unexpected Error", "Value Error", tokens);
            }
        }

    }
    
    // cout<<"result: "<<result;
    // reset the result value
    if(result == 0){
        return false;
    }else if(result == 1){
        return true;
    }else if(isInteger(result)){
        return static_cast<int>(result);
    }else if(!isInteger(result)){
        return static_cast<float>(result);
    }

        
}
