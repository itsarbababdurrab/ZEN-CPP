
#include <iostream>
#include <vector>
#include <variant>
#include "main.hpp"
#include "symbolTable.hpp"
#include "lexer.hpp"
#include "errorHandler.hpp"
using namespace std;

SymbolTable st;

void SymbolTable::create(const vector<MainClass::dataTypes_variant>& tokens, int line_num){

    MainClass mc;
    ErrorHandler err_h;
    // set dictionary name
    string dict_name = mc.trim(get<string>(tokens[1]));
    MainClass::dataTypes_variant value = tokens.back();

    if(st.stm_dict.find(dict_name) != st.stm_dict.end()){
        // throw error for initializing existing variable
        // cout<<"already existing variable"<<endl;
        vector<string> tkns;
        err_h.handleError(line_num, "Initialization of Existing Identifier", "Value Error", tkns);
        
    }else{
        // cout<<"inserting :"<<dict_name<<endl;
        // insert new symbol into the table
        st.stm_dict[dict_name] = {
            get<string>(tokens[0]),   //data type
            dict_name, // identifier
            value, // value
            typeid(tokens.back()).name() // type of value
        };
    }
}

MainClass::dataTypes_variant SymbolTable::retrieve(const vector<string>& tokens, const string& variable,const string& info_need, int line_num){
    MainClass mc;
    ErrorHandler err_h;
    
    MainClass::dataTypes_variant result;
    auto it = st.stm_dict.find(variable);
    // cout<<"variable to retrieve :"<<variable<<endl;
    if(it != st.stm_dict.end()){
        // cout<<"found"<<endl;
        if(info_need == "value"){
            // return the value
            result = it->second.value;
           
        }else if(info_need == "type"){
            // return the type
            result = it->second.dataType;
        }
    }else{
        // throw error
        // cout<<"value is not in st"<<endl;
        err_h.handleError(line_num, "Accessing Variable before its Declaration", "Value Error", tokens);
        return {};
    }
    // cout<<"Result is: "<<result<<endl;
    // return result;
    if(holds_alternative<int>(result)){
        return get<int>(result);
    }else if(holds_alternative<float>(result)){
        return get<float>(result);
    }else if(holds_alternative<string>(result)){
        return get<string>(result);
    }else if(holds_alternative<bool>(result)){
        return get<bool>(result);
    }else{
        
    }
}
