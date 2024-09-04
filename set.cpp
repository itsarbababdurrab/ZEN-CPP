#include <iostream>
#include <string>
#include <cmath>
#include <regex>
#include "set.hpp"
#include "expressionHandler.hpp"
#include "main.hpp"
#include "errorHandler.hpp"
#include "symbolTable.hpp"
using namespace std;

bool Set::isInteger(double value){
    return floor(value) == value;
}

void Set::printValue(const string& message, const variant<int, float, double, string, bool>& value) {
    std::visit([&](auto&& val) {
        if(!message.empty()){
            if constexpr (is_same_v<decay_t<decltype(val)>, bool>){
                if(val == 1){
                    cout << message << " " << "true" << endl;
                }else{
                    cout << message << " " << "false" << endl;
                }
            }
        }else if(message.empty()){
            if constexpr (is_same_v<decay_t<decltype(val)>, bool>){
                if(val == 1){
                    cout << "true" << endl;
                }else{
                    cout << "false" << endl;
                }
            }
        }
    }, value);
}

void Set::set_user(vector<string> tokens, int line_num, string message, string variable, string exp){
    ExpressionHandler exp_h;
    ErrorHandler err_h;
    // check if variable or message is avail
    if(!variable.empty() && !message.empty()){
        // get result from expression handler
        MainClass::dataTypes_variant result = exp_h.exp_handler(tokens, line_num, variable);
        if(holds_alternative<int>(result)){
            printValue(message, get<int>(result));
        }else if(holds_alternative<float>(result)){
            printValue(message, get<float>(result));
        }else if(holds_alternative<bool>(result)){
            if(get<bool>(result) == 1){
                printValue(message, true);
            }else{
                printValue(message, false);
            }
        }else{
            err_h.handleError(line_num, "Unexpected Error", "Value Error", tokens);
        }
    }else if(!message.empty()){
        cout<<message<<endl;
    }else if(!variable.empty()){
        MainClass::dataTypes_variant result = exp_h.exp_handler(tokens, line_num, variable);
        if(holds_alternative<int>(result)){
            cout<<get<int>(result)<<endl;
        }else if(holds_alternative<float>(result)){
            cout<<get<float>(result)<<endl;
        }else if(holds_alternative<bool>(result)){
            if(get<bool>(result) == 1){
                cout<<"true"<<endl;
            }else{
                cout<<"false"<<endl;
            }
        }else{
            err_h.handleError(line_num, "Unexpected Error", "Value Error", tokens);
        }
    }

    if(!exp.empty()){
        if(regex_search(exp, regex("[+-\\/*%]"))){
            MainClass::dataTypes_variant result = exp_h.exp_handler(tokens, line_num, exp);
            if(holds_alternative<int>(result)){
                cout<<get<int>(result)<<endl;
            }else if(holds_alternative<float>(result)){
                cout<<get<float>(result)<<endl;
            }else if(holds_alternative<bool>(result)){
                if(get<bool>(result) == 1){
                    cout<<"true"<<endl;
                }else{
                    cout<<"false"<<endl;
                }
            }
        }else{
            SymbolTable st;
            MainClass::dataTypes_variant result = st.retrieve(tokens, exp, "value", line_num);
            if(holds_alternative<int>(result)){
                cout<<get<int>(result)<<endl;
            }else if(holds_alternative<float>(result)){
                cout<<get<float>(result)<<endl;
            }else if(holds_alternative<bool>(result)){
                if(get<bool>(result) == 1){
                    cout<<"true"<<endl;
                }else{
                    cout<<"false"<<endl;
                }
            }
        }
    }
}