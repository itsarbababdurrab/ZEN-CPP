#include <iostream>
#include <string>
#include <vector>
#include "main.hpp"
#include "lexer.hpp"
#include "semanticAnalyzer.hpp"
#include "symbolTable.hpp"
#include "expressionHandler.hpp"
#include "errorHandler.hpp"
#include "set.hpp"
#include "get.hpp"
using namespace std;


bool SemanticAnalyzer::startswith(const string& str, char c){
    return !str.empty() && str.front() == c;
}
bool SemanticAnalyzer::endswith(const string& str, char c){
    return !str.empty() && str.back() == c;
}

string SemanticAnalyzer::concacteTokens(const vector<MainClass::dataTypes_variant> tokens, size_t start, size_t end){

    string joined;
    for(size_t i = start; i<end; ++i){
        if(!joined.empty()){
            joined+= " "; // add space after a word
        }
        joined+=get<string>(tokens[i]);
    }
    return joined;
}

bool Executing = true;
bool require_open_bracket = false;
bool require_close_bracket = false;
bool if_flag = false;
bool else_flag = false;




void SemanticAnalyzer::semantic_analyzer(vector<string>& tkns, int line_num){
    // Lexer lxr;
    // for(size_t i =0; i< tokens.size(); ++i){
    //     lxr.prnt(tokens[i]);
    // }

    SemanticAnalyzer sa;
    MainClass mc;
    SymbolTable st;
    ExpressionHandler eh;
    ErrorHandler err_h;
    Get user;

    
    // remove leading and trailling whitespaces from each token
    vector<string> tokens;
    for(size_t i = 0; i<tkns.size(); ++i){
        tokens.push_back(mc.trim(tkns[i]));
    }
    // 'Executing' variable for executing the body of the if statement
    // if 'Executing' is true the expressions/ Statements will be executed other wise no execution

    
    vector<MainClass::dataTypes_variant> analyzed_tokens;


    if(tokens[0] == "{" && require_open_bracket && tokens.size() == 1){
        require_open_bracket = false;
    }else if(tokens[0] == "}" && require_close_bracket && tokens.size() == 1){
        if(else_flag){
            else_flag = false;
            if(!Executing){
                Executing = true;
            }
        }
        require_close_bracket = false;
    }else if((find(mc.dataTypes.begin(), mc.dataTypes.end(), tokens[0]) != mc.dataTypes.end()) && Executing){
        mc.trim(tokens.back());
        // validate the variable name
        if(regex_match(tokens[1], regex(mc.variable_regex))){
            /* 
                The value(after '=' sign) can either be:  
                    1. direct value
                    2. variable 
                    3. expression (if Num or Decimal)
                        1. 1+2
                        2. a+1
                        3. 1+a
                        4. a+a
                    4. Get<""    
            */

            if(tokens[0] == "Num"){
                if(regex_search(tokens.back(), regex("[<]"))){
                    variant<int, float, string, bool> user_in = user.get_user(line_num, tokens, tokens.back());
                    if(holds_alternative<int>(user_in)){
                        analyzed_tokens = {tokens[0], tokens[1], get<int>(user_in)};
                    }else if(holds_alternative<bool>(user_in)){
                        if(get<bool>(user_in) == 1){
                            analyzed_tokens = {tokens[0], tokens[1], 1};
                        }else{
                            analyzed_tokens = {tokens[0], tokens[1], 0};
                        }
                    }else{
                        err_h.handleError(line_num, "Invalid Assignment to Num", "Value Error", tokens);
                    }
                }else if(regex_match(tokens.back(), regex(mc.int_regex))){
                    analyzed_tokens = {tokens[0], tokens[1], stoi(tokens.back())};
                }else if(regex_match(tokens.back(), regex(mc.variable_regex))){
                    
                    // retrieve variable value from symbol table
                    MainClass::dataTypes_variant value = st.retrieve(tokens, tokens.back(), "value", line_num);
                    
                    if(!holds_alternative<int>(value)){
                        if(holds_alternative<bool>(value)){
                            if(get<bool>(value) == 1 || get<bool>(value) == 0){
                                analyzed_tokens = {tokens[0], tokens[1], static_cast<int>(get<bool>(value))};
                            }else{
                                // cout<<"not int value"<<endl; //////////////////////////////////////////////////////
                                err_h.handleError(line_num, "Invalid Assignment to Num", "Value Error", tokens);
                            }
                        }else{
                            // error for invalid assignment
                            // cout<<"invalid assignment of "<<get<string>(value)<<" to Num"<<endl;
                            err_h.handleError(line_num, "Invalid Assignment to Num", "Value Error", tokens);
                        }
                    }else{
                        
                        analyzed_tokens = {tokens[0], tokens[1], get<int>(value)};
                    }

                }else if(regex_search(tokens.back(), regex("[a-zA-Z\\d_\\.+-\\/*()]"))){

                    // call the expression handle
                    MainClass::dataTypes_variant result = eh.exp_handler(tokens, line_num, tokens.back());
                    // cout<<endl<<"result type is : "<<typeid(result).name();
                    if(!holds_alternative<int>(result)){
                        if(holds_alternative<bool>(result)){
                            if(get<bool>(result) == 1 || get<bool>(result) == 0){
                                analyzed_tokens = {tokens[0], tokens[1], static_cast<int>(get<bool>(result))};
                            }else{
                                // cout<<"not int value"<<endl; //////////////////////////////////////////////////////
                                err_h.handleError(line_num, "Invalid Assignment to Num", "Value Error", tokens);
                            }
                        }else{
                            // error for invalid assingment
                            err_h.handleError(line_num, "Invalid Assignment to Num", "Value Error", tokens);
                        }
                    }else{
                        // cout<<"result is: "<<static_cast<int>(result);
                        analyzed_tokens = {tokens[0], tokens[1], get<int>(result)};
                    }

                }else{
                    // throw error for invalid assignment
                    err_h.handleError(line_num, "Invalid Assignment to Num", "Value Error", tokens);
                }
            }else if(tokens[0] == "Decimal"){
                if(regex_search(tokens.back(), regex("[<]"))){
                   variant<int, float, string, bool> user_in = user.get_user(line_num, tokens, tokens.back());
                    if(holds_alternative<float>(user_in)){
                        analyzed_tokens = {tokens[0], tokens[1], get<float>(user_in)};
                    }if(holds_alternative<int>(user_in)){
                        analyzed_tokens = {tokens[0], tokens[1], get<int>(user_in)};
                    }else{
                        err_h.handleError(line_num, "Invalid Assignment to Decimal", "Value Error", tokens);
                    }

                }else if(regex_match(tokens.back(), regex(mc.float_regex))){

                    analyzed_tokens = {tokens[0], tokens[1], stof(tokens.back())};

                }else if(regex_match(tokens.back(), regex(mc.variable_regex))){
                    
                    // retrieve variable value from symbol table
                    MainClass::dataTypes_variant value = st.retrieve(tokens, tokens.back(), "value", line_num);
                    
                    if(!holds_alternative<float>(value)){
                        if(holds_alternative<bool>(value)){
                            if(get<bool>(value) == 1 || get<bool>(value) == 0){
                                analyzed_tokens = {tokens[0], tokens[1], static_cast<int>(get<bool>(value))};
                            }else{
                                // cout<<"not int value"<<endl; //////////////////////////////////////////////////////
                                err_h.handleError(line_num, "Invalid Assignment to Decimal", "Value Error", tokens);
                            }
                        }else{
                            // error for invalid assingment
                            err_h.handleError(line_num, "Invalid Assignment to Decimal", "Value Error", tokens);
                        }
                    }else{
                        // cout<<"result is: "<<static_cast<int>(result);
                        analyzed_tokens = {tokens[0], tokens[1], get<float>(value)};
                    }

                }else if(regex_search(tokens.back(), regex("'")) || regex_search(tokens.back(), regex("\""))){
                    // throw error for invalid assignment
                    // cout<<"invlid ass to float"<<endl;
                    err_h.handleError(line_num, "Invalid Assignment to Decimal", "Value Error", tokens);
                }else if(regex_search(tokens.back(), regex("[a-zA-Z\\d_\\.+-\\/*()]"))){

                    // call the expression handle
                    MainClass::dataTypes_variant result = eh.exp_handler(tokens, line_num, tokens.back());
                    if(!holds_alternative<float>(result)){
                        if(holds_alternative<bool>(result)){
                            if(get<bool>(result) == 1 || get<bool>(result) == 0){
                                analyzed_tokens = {tokens[0], tokens[1], static_cast<float>(get<bool>(result))};
                            }else{
                                // cout<<"not int value"<<endl; //////////////////////////////////////////////////////
                                err_h.handleError(line_num, "Invalid Assignment to Decimal", "Value Error", tokens);
                            }
                        }else if(holds_alternative<int>(result)){

                            // cout<<"result is: "<<static_cast<int>(result);
                            analyzed_tokens = {tokens[0], tokens[1], static_cast<float>(get<int>(result))};
                        }
                    }else{
                        // error for invalid assingment
                        err_h.handleError(line_num, "Invalid Assignment to Decimal", "Value Error", tokens);
                    }

                }
            }else if(tokens[0] == "Literal"){

                if(regex_search(tokens.back(), regex("[<]"))){
                    variant<int, float, string, bool> user_in = user.get_user(line_num, tokens, tokens.back());
                    if(holds_alternative<string>(user_in)){
                        analyzed_tokens = {tokens[0], tokens[1], get<string>(user_in)};
                    }else{
                        err_h.handleError(line_num, "Invalid Assignment to Literal", "Value Error", tokens);
                    }
                }else if(regex_match(tokens.back(), regex(mc.variable_regex))){
                    MainClass::dataTypes_variant value = st.retrieve(tokens, tokens.back(), "value", line_num);
                    if(holds_alternative<string>(value)){
                        analyzed_tokens = {tokens[0], tokens[1], get<string>(value)};
                    }else{
                        // cout<<"Not string value in st"<<endl;
                        err_h.handleError(line_num, "Invalid Assignment to Literal", "Value Error", tokens);
                    }
                }else{
                    // check whether string is directly assigned to Literal
                    if((startswith(tokens.back(), '\'') && endswith(tokens.back(), '\'')) || (startswith(tokens.back(), '"') && endswith(tokens.back(), '"'))){
                        string final_string;
                        // remove quotes from the string
                        if(startswith(tokens.back(), '"') && endswith(tokens.back(), '"')){
                            string last_token = tokens.back();
                            last_token.erase(remove(last_token.begin(), last_token.end(), '"'), last_token.end());
                            final_string = mc.trim(last_token);
                        }else if(startswith(tokens[tokens.size()-1], '\'') && endswith(tokens[tokens.size()-1], '\'')){
                            string last_token =tokens.back();
                            last_token.erase(remove(last_token.begin(), last_token.end(), '\''), last_token.end());
                            final_string = mc.trim(last_token);
                        }
                        analyzed_tokens={tokens[0], tokens[1], final_string};
                        
                    }else{
                        // throw error////////////////////////////////////////////////////////////
                        err_h.handleError(line_num, "Invalid Assignment to Literal", "Value Error", tokens);
                    }
                }

            }else if(tokens[0] == "Bool"){
                string last_token = mc.trim(tokens.back());
                if(regex_search(last_token, regex("[<]"))){
                    variant<int, float, string, bool> user_in = user.get_user(line_num, tokens, last_token);
                    if(holds_alternative<bool>(user_in)){
                        if(get<bool>(user_in) == 1){
                            analyzed_tokens = {tokens[0], tokens[1], "true"};
                        }else{
                            analyzed_tokens = {tokens[0], tokens[1], "false"};
                        }
                    }else{
                        err_h.handleError(line_num, "Invalid Assignment to Num", "Value Error", tokens);
                    }

                }else if(regex_match(last_token, regex("([a-zA-Z_][a-zA-Z_\\d]*|\\d+)\\s*(<=|>=|==|!=|<|>)\\s*([a-zA-Z_][a-zA-Z_\\d]*|\\d+)"))){
                    MainClass::dataTypes_variant result = eh.exp_handler(tokens, line_num, last_token);
                    if(holds_alternative<bool>(result)){
                        bool res = get<bool>(result);
                        if(res == 1){
                            analyzed_tokens = {tokens[0], tokens[1], true};                    
                        }else if(res == 0){
                            analyzed_tokens = {tokens[0], tokens[1], false};                    
                        }
                    }else{
                        // throw error
                        err_h.handleError(line_num, "Invalid Assignment to Bool", "Value Error", tokens);
                    }
                    

                }else if(last_token == "true" || last_token == "false"){
                    if(last_token == "true"){
                        analyzed_tokens = {tokens[0], tokens[1], true};
                    }else{
                        analyzed_tokens = {tokens[0], tokens[1], false};
                    }
                }else{
                    // throw error ///////////////////////////////////////////////////////////
                    err_h.handleError(line_num, "Invalid Assignment to Bool", "Value Error", tokens);
                }
            }
           
        }else{
            // throw error for var name
            // cout<<"invalid var"<<endl;
            err_h.handleError(line_num, "Invalid Identifier", "value Error", tokens);
        }

        // pass analyzed tokens to Symbol Table
        if(!analyzed_tokens.empty()){
            // cout<<"Passing to create"<<endl;
            Lexer lxr;
            for(size_t i; i<analyzed_tokens.size(); ++i){
                lxr.prnt(analyzed_tokens[i]);
            }
            st.create(analyzed_tokens, line_num);
        }else{
            err_h.handleError(line_num, "Unexpected Error", "value Error", tokens);
        }
        // Lexer lxr;
        // for(const auto& token: analyzed_tokens){
        //     lxr.prnt(token);
        // }

    }else if((find(mc.grammarSymbols.begin(), mc.grammarSymbols.end(), tokens[0]) != mc.grammarSymbols.end()) && Executing){
        Set set;
        if(tokens[0] == "Set"){
            bool variable = false;
            bool in_quotes = false;
            string variable_name = "";
            string temp_message = "";
            char quote_sign;

            if(startswith(tokens.back(), '\'') || startswith(tokens.back(), '"')){
                for(char ch: tokens.back()){
                    if((ch == '"' || ch =='\'') && !in_quotes){
                        in_quotes = true;
                        quote_sign = ch;
                        temp_message+=ch;
                    }else if(ch != quote_sign && in_quotes){
                        temp_message+=ch;
                    }else if(ch == quote_sign && in_quotes){
                        in_quotes = false;
                        temp_message+=ch;
                    }else if(ch == '>' && !in_quotes && !variable){
                        variable = true;
                    }else if(ch != ' ' && variable){
                        variable_name+=ch;
                    }else if(ch != ' ' && !variable){
                        // throw error////////////////////////////////////// unexpected
                        err_h.handleError(line_num, "Unexpected error", "value Error", tokens);
                    }else if(string("+-/*").find(ch) != string::npos){
                        variable_name+=ch;
                    }
                }
            }else{
                // if an expression or variable only
                set.set_user(tokens, line_num, "", "", tokens.back());
            }

            if(!temp_message.empty() || !variable_name.empty()){
                if(variable_name == "" && variable){
                    // throw error ////////////////////////////////missing value
                    err_h.handleError(line_num, "Missing Value", "value Error", tokens);
                }else{
                    if(!temp_message.empty()){
                        string msg = temp_message;
                        msg.erase(remove(msg.begin(), msg.end(), '"'), msg.end());
                        string final_string = mc.trim(msg);
                        // call set for the string //////////////////////////////
                        set.set_user(tokens, line_num, final_string, variable_name, "");
                    }else{
                        set.set_user(tokens, line_num, temp_message, variable_name, "");
                    }
                }
            }
        
        }else{
            // do nothing for:- Get<"a message to display..........."
        }
        
    }else if(tokens[0] == "if" && Executing){
        if_flag = true;
        if(count(tokens[2].begin(), tokens[2].end(), '&') > 1 || count(tokens[2].begin(), tokens[2].end(), '|') > 1){
            // throw error for exceeding number of logical symbol &, |
            err_h.handleError(line_num, "Invalid Logical Syntax", "Syntax Error", tokens);
        }else{
            if(tokens.back() == "{"){
                require_open_bracket = false;
                require_close_bracket = true;
                if(regex_search(tokens[2], regex("[&|]"))){
                    string temp_exp = "";
                    string expression = "";
                    for(char ch: tokens[2]){
                        if(ch == '&' || ch == '|'){
                            if(!temp_exp.empty() && regex_search(temp_exp, regex("[!<>=]"))){
                                MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, temp_exp);
                                if(holds_alternative<bool>(res)){
                                    bool result = get<bool>(res);
                                    if(result == 1){
                                        expression+="true";
                                    }else{
                                        expression+="false";
                                    }
                                    expression+=ch;
                                }else{
                                    // throw error
                                    err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                                }
                                temp_exp.clear();
                            }else{
                                expression+=ch;
                            }
                        }else if(ch == ' '){

                        }else{
                            temp_exp+=ch;
                        }
                    }

                    if(!temp_exp.empty()){
                        if(regex_search(temp_exp, regex("[!<>=]"))){
                            MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, temp_exp);
                            if(holds_alternative<bool>(res)){
                                bool result = get<bool>(res);
                                if(result == 1){
                                    expression+="true";
                                }else{
                                    expression+="false";
                                }
                            }else{
                                // throw error
                                err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                            }
                            temp_exp.clear();
                        }else{
                            expression+=temp_exp;
                            temp_exp.clear();
                        }
                    }

                    if(!expression.empty()){
                        MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, expression);
                        if(holds_alternative<bool>(res)){
                            bool result = get<bool>(res);
                            Executing = result;
                        }else{
                            // throw error
                            err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                        }
                    }

                }else{
                    if(regex_search(tokens[2], regex("[!<>=]"))){
                        MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, tokens[2]);
                        if(holds_alternative<bool>(res)){
                            bool result = get<bool>(res);
                            Executing = result;
                        }else{
                            // throw error
                            err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                        }
                    }else if(regex_match(tokens[2], regex(mc.variable_regex))){
                        MainClass::dataTypes_variant result = st.retrieve(tokens, tokens[2], "value", line_num);
                        if(holds_alternative<bool>(result)){
                            bool value = get<bool>(result);
                            Executing = value;
                        }else{
                            err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                        }
                    }
                }
            }else if(tokens.back() == ")"){
                require_open_bracket = true;
                require_close_bracket = true;

                if(regex_search(tokens[2], regex("[&|]"))){
                    string temp_exp = "";
                    string expression = "";
                    for(char ch: tokens[2]){
                        if(ch == '&' || ch == '|'){
                            if(!temp_exp.empty() && regex_search(temp_exp, regex("[!<>=]"))){
                                MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, temp_exp);
                                if(holds_alternative<bool>(res)){
                                    bool result = get<bool>(res);
                                    if(result == 1){
                                        expression+="true";
                                    }else{
                                        expression+="false";
                                    }
                                    expression+=ch;
                                }else{
                                    // throw error
                                    err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                                }
                                temp_exp.clear();
                            }else{
                                expression+=ch;
                            }
                        }else if(ch == ' '){

                        }else{
                            temp_exp+=ch;
                        }
                    }
                    if(!temp_exp.empty()){
                        if(regex_search(temp_exp, regex("[!<>=]"))){
                            MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, temp_exp);
                            if(holds_alternative<bool>(res)){
                                bool result = get<bool>(res);
                                if(result == 1){
                                    expression+="true";
                                }else{
                                    expression+="false";
                                }
                            }else{
                                // throw error
                                err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                            }
                            temp_exp.clear();
                        }else{
                            expression+=temp_exp;
                            temp_exp.clear();
                        }
                    }

                    if(!expression.empty()){
                        MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, expression);
                        if(holds_alternative<bool>(res)){
                            bool result = get<bool>(res);
                            Executing = result;
                        }else{
                            // throw error
                            err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                        }
                    }

                }else{
                    if(regex_search(tokens[2], regex("[!<>=]"))){
                        MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, tokens[2]);
                        if(holds_alternative<bool>(res)){
                            bool result = get<bool>(res);
                            Executing = result;
                        }else{
                            err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                        }
                    }else if(regex_match(tokens[2], regex(mc.variable_regex))){
                        MainClass::dataTypes_variant res = st.retrieve(tokens, tokens[2], "value", line_num);
                        if(holds_alternative<bool>(res)){
                            bool value = get<bool>(res);
                            Executing = value;
                        }else{
                            // throw error
                            err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                        }
                    }
                }
            }
        }
    }else if(tokens[0] == "else" || (tokens[0] == "}" && require_close_bracket)){
        if(!Executing){
            Executing = true;
        }else{
            Executing = false;
        }

        if(tokens.size() == 1){
            if(tokens[0] == "else" && tokens.size() == 1 && if_flag){
                else_flag = true;
                require_open_bracket = true;
                require_close_bracket = true;
            }
        }else if(tokens.size() > 1){
            // }else{
            if(tokens[0] == "}" && require_close_bracket){
                require_close_bracket = false;
                if(tokens[1] == "else" && if_flag){
                    else_flag = true;
                    if(tokens[2] == "if"){
                        // throw error //////////syntax
                        err_h.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                    }
                    if(tokens.back() == "{"){
                        require_open_bracket = false;
                        require_close_bracket = true;
                    }else{
                        require_open_bracket = true;
                        require_close_bracket = true;
                    }
                }else{
                    // throw error////////////////syntax
                    err_h.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                }
            }else if(tokens[0] == "else" && if_flag){
                else_flag = true;
                if(tokens[1] == "if"){
                    // throw error ///////////////////syntax
                    err_h.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                }
                if(tokens.size() > 1){
                    if(tokens[1] == "{"){
                        require_open_bracket = false;
                        require_close_bracket = true;
                    }else{
                        // throw error //////////////////syntax
                        err_h.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                    }
                }else if(tokens.size() == 1){
                    if(tokens[0] == "else"){
                        require_open_bracket = true;
                        require_close_bracket = true;
                    }
                } 
            }else{
                cout<<"errr";
            }
        }
    
    }else if((tokens[0] == "{" && !require_open_bracket) || (tokens[0] == "}" && !require_close_bracket)){
        // throw error /////////////syntax  
        err_h.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);  
    }else if(tokens.back() == "?" && Executing){
        // for(const auto& token: tokens){
        //     cout<<token<<endl;
        // }
        
        if(tokens.size() == 2){
            if(count(tokens[0].begin(), tokens[0].end(), '&') > 1 || count(tokens[0].begin(), tokens[0].end(), '|') > 1){
                // throw error for exceeding number of logical symbol &, |
                err_h.handleError(line_num, "Invalid Logical Syntax", "Syntax Error", tokens);
            }else{
                string temp_exp = "";
                string expression = "";
                for(char ch: tokens[0]){
                    if(ch == '&' || ch == '|'){
                        if(!temp_exp.empty() && regex_search(temp_exp, regex("[!<>=]"))){
                            MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, temp_exp);
                            // cout<<typeid(res).name()<<endl;
                            if(holds_alternative<bool>(res)){
                                bool result = get<bool>(res);
                                if(result == 1){
                                    expression+= "true";
                                    expression+=ch;
                                }else if(result == 0){
                                    expression+= "false";
                                    expression+=ch;
                                }
                            }else{
                                // throw error
                                err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                            }
                            temp_exp.clear();
                        }else{
                            expression+=ch; 
                        }
                    }else if(ch == ' '){

                    }else{
                        temp_exp+=ch;
                    }
                }

                if(!temp_exp.empty()){
                    if(regex_search(temp_exp, regex("[!<>=]"))){
                        MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, temp_exp);
                        if(holds_alternative<bool>(res)){
                            bool result = get<bool>(res);
                            if(result == 1){
                                expression+="true";
                            }else if(result == 0){
                                expression+="false";
                            }
                        }else{
                            // throw error
                            err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                        }
                        temp_exp.clear();
                    }else{
                        expression+=temp_exp;
                    }
                }

                if(!expression.empty()){
                    // cout<<"expression: "<<expression<<endl;
                    MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, expression);
                    if(holds_alternative<bool>(res)){
                        bool result = get<bool>(res);
                        if(result == 1){
                            cout<<"true"<<endl;
                        }else if(result == 0){
                            cout<<"false"<<endl;
                        }
                    }else{
                        // throw error
                        err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                    }
                }

            }
        
        }else{
            if(regex_match(tokens[0], regex(mc.int_float_regex))){
                if(regex_match(tokens[tokens.size()-2], regex(mc.int_float_regex))){
                    analyzed_tokens = {tokens[0], tokens[1], tokens[tokens.size()-2]};
                }else if(regex_match(tokens[tokens.size()-2], regex(mc.variable_regex))){
                    MainClass::dataTypes_variant value = st.retrieve(tokens, tokens[tokens.size()-2], "value", line_num);
                    if(holds_alternative<int>(value)){
                        analyzed_tokens = {tokens[0], tokens[1], value};
                    }else if(holds_alternative<float>(value)){
                        analyzed_tokens = {tokens[0], tokens[1], value};
                    }
                }else{
                    // throw error////////// missing value
                    err_h.handleError(line_num, "Missing Value", "Value Error", tokens);
                }
            }else if(regex_match(tokens[0], regex(mc.variable_regex))){
                // cout<<"looking for "<<tokens[0]<<endl;
                MainClass::dataTypes_variant value1 = st.retrieve(tokens, tokens[0], "value", line_num);
                
                if(regex_match(tokens[tokens.size()-2], regex(mc.int_float_regex))){
                    // analyzed_tokens = {value1, tokens[1], tokens[2]};
                    if(holds_alternative<int>(value1)){
                        int val1 = get<int>(value1);
                        analyzed_tokens = {to_string(val1), tokens[1], static_cast<string>(tokens[2])};
                    }else if(holds_alternative<float>(value1)){
                        float val1 = get<float>(value1);
                        analyzed_tokens = {to_string(val1), tokens[1], static_cast<string>(tokens[2])};
                    }
                }else if(regex_match(tokens[tokens.size()-2], regex(mc.variable_regex))){
                    MainClass::dataTypes_variant value2 = st.retrieve(tokens, tokens[tokens.size()-2], "value", line_num);
                    // analyzed_tokens = {value1, tokens[1], value2};
                    if(holds_alternative<int>(value2)){
                        int val2 = get<int>(value2);
                        if(holds_alternative<int>(value1)){
                            int val1 = get<int>(value1);
                            analyzed_tokens = {to_string(val1), tokens[1], to_string(val2)};
                        }else if(holds_alternative<float>(value1)){
                            float val1 = get<float>(value1);
                            analyzed_tokens = {to_string(val1), tokens[1], to_string(val2)};
                        }
                        // analyzed_tokens = {get<int>(value1), tokens[1], get<int>(value2)};
                    }else if(holds_alternative<float>(value2)){
                        float val2 = get<float>(value2);
                        // analyzed_tokens = {get<float>(value1), tokens[1], get<float>(value2)};
                        if(holds_alternative<int>(value1)){
                            int val1 = get<int>(value1);
                            analyzed_tokens = {to_string(val1), tokens[1], to_string(val2)};
                        }else if(holds_alternative<float>(value1)){
                            float val1 = get<float>(value1);
                            analyzed_tokens = {to_string(val1), tokens[1], to_string(val2)};
                        }
                    }
                }else{
                    // throw error ///////////////////missing value
                    // cout<<"error"<<endl;
                    err_h.handleError(line_num, "Missing Value", "Value Error", tokens);
                }
            }

            if(!analyzed_tokens.empty()){
                // Lexer lxr;
                // for(const auto token : analyzed_tokens){
                //     lxr.prnt(token);
                // }
                string exp = sa.concacteTokens(analyzed_tokens, 0, analyzed_tokens.size());
                // cout<<exp<<endl;
                MainClass::dataTypes_variant res = eh.exp_handler(tokens, line_num, exp);
                if(holds_alternative<bool>(res)){
                    bool result = get<bool>(res);
                    if(result == 1){
                        cout<<"true"<<endl;
                    }else if(result == 0){
                        cout<<"false"<<endl;
                    }
                }else{
                    // throw error
                    // cout<<"not bool return"<<endl;
                    err_h.handleError(line_num, "Unexpected Value", "Value Error", tokens);
                }
            }
        }
    }
}