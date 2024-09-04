#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include "get.hpp"
#include <regex>
#include "errorHandler.hpp"
#include "main.hpp"

using namespace std;

vector<string> Get::tokenize(const string& content) {

    // tokenize the line and call the parser to create AST
    // The Regex for tokenizing
    const string pattern = "[a-zA-Z\\d_<\'\"]+";
    // match the input string
    vector<string> tkns;
    // create regex obj with the pattern
    regex re(pattern);
    auto wordsbegin = sregex_iterator(content.begin(), content.end(), re);
    auto wordsend = sregex_iterator();  

    for(sregex_iterator i = wordsbegin; i != wordsend; i++){
        smatch match = *i;
        tkns.push_back(match.str());
    }
    return tkns;
}

variant<int, float, string, bool> Get::get_user(int line_num, vector<string> tokens, string last_token){
    
    ErrorHandler error_handler;
    MainClass mc;
    vector<string> tkns = tokenize(last_token);

    string user_input;

    if(tkns[0] == "Get"){
        if(tkns[1] == "<"){
            // Join tokens from index 2 to the end into a single string
            ostringstream oss;
            for(size_t i=2; i<tkns.size(); ++i){
                if(i != 2) oss <<" ";
                oss << tkns[i];
            }
            string message = oss.str();
            // check if message start and end with quotes
            if((message.front() == '"' && message.back() == '"') || (message.front() == '\'' && message.back() == '\'')){
                bool in_quotes = false;
                char quote_sign = '\0';
                string temp_message = "";

                for (char ch : message) {
                    if ((ch == '"' || ch == '\'') && !in_quotes) {
                        quote_sign = ch;
                        in_quotes = true;
                    } else if (ch != quote_sign && in_quotes) {
                        temp_message += ch;
                    } else if (ch == quote_sign && in_quotes) {
                        in_quotes = false;
                    }
                }
                // Check if msg starts and ends with the same quote_sign
                if(message.front() == quote_sign && message.back() == quote_sign){
                    // accept user input
                    // string user_input;
                    cout<<temp_message<<endl;
                    getline(cin, user_input);
                }else{
                    error_handler.handleError(line_num, "Invalid Quotes", "Value Error", tokens);
                }
            }else{
                error_handler.handleError(line_num, "Unexpected Error", "Value Error", tokens);
            }
        }else{
            error_handler.handleError(line_num, "Missing <", "Syntax Error", tokens);
        }
    }else{
        error_handler.handleError(line_num, "Missing name 'Get'", "Value Error", tokens);
    }
    // cout<<"user val: "<<typeid(user_input).name()<<endl;
    if(user_input == "1"){
        return 1;
    }else if(user_input == "0"){
        return 0;
    }else if(regex_match(user_input, regex(mc.int_regex))){
        return stoi(user_input);
    }else if(regex_match(user_input, regex(mc.float_regex))){
        return stof(user_input);
    }else{
        return user_input;
    }
}