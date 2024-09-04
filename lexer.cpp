

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include "main.hpp"
#include "lexer.hpp"
#include "syntaxAnalyzer.hpp"
#include "errorHandler.hpp"
using namespace std;


vector<string> Lexer::tokenize(const string& content) {
    
    // tokenize the line and call the parser to create AST
    // The Regex for tokenizing
    const string pattern = R"(([a-zA-Z\d_$#@%^;:~`*+\-/\'"\.]+)|([=])|([{}])|([()])|([|])|([&])|(['"])|([<>])|([!])|([?]))";
    // match the input string
    vector<string> tokens;
    // create regex obj with the pattern
    regex re(pattern);
    auto wordsbegin = sregex_iterator(content.begin(), content.end(), re);
    auto wordsend = sregex_iterator();  

    for(sregex_iterator i = wordsbegin; i != wordsend; i++){
        smatch match = *i;
        tokens.push_back(match.str());
    }
    return tokens;
}

void Lexer::prnt(const MainClass::dataTypes_variant& var) {
    // Use std::visit to determine the active type in the variant
    std::visit([](auto&& arg) {
        cout << "Type: " << typeid(arg).name() << ", Value: " << arg << endl;
    }, var);
}

void Lexer::lexicalAnalyzer(string& content, int line_num, vector<string>& dataTypes, vector<string>& grammarSymbols){
    // cout<<"contents revieved:"<<content;

    MainClass mc;
    SyntaxAnalyzer sa;
    /* The lexer will clean the input from unnecessary whitespaces and empty lines */

    vector<string> lexical_tokens;
    // pass the contents to tokenize function
    vector<string> tokens = tokenize(content);
    /* Handle Comments */
    if(tokens[0] == "|" && tokens.back() == "|"){
        // Ignore comments or make some styling.....................................................
        cout<<"the line is a comment";
        return;
    }

    
    // for(const auto& token: scanned_tokens){
    //     prnt(token);
    // }
    
    // Further analyze the scanned tokens and provide them to parser/syntax analyzer
    
    if(tokens.size() > 1 || (tokens.size() == 1 && tokens[0] == "else")){
        /* prepare the tokens according to the pre defined rules for each dataType
        e.g.
        datatype var = value || datatype var = 2+1 || datatype var = var+2 || datatype var = var+var || datatype var = get('string')
        it means the length of the value is not known, so we will combine all the tokens after the '=' symbol */

        if(find(dataTypes.begin(), dataTypes.end(), tokens[0]) != dataTypes.end()){ 
            string last_token = mc.concacte(tokens, 3, tokens.size());
            lexical_tokens = {tokens[0], tokens[1], tokens[2], last_token};
            last_token.clear();

        }else if(find(grammarSymbols.begin(), grammarSymbols.end(), tokens[0]) != grammarSymbols.end()){
            string last_token = mc.concacte(tokens, 2, tokens.size());
            lexical_tokens = {tokens[0], tokens[1], last_token};

        }else if(tokens[0] == "if"){
            if(tokens.back() == "{"){
                string exp = mc.concacte(tokens, 2, tokens.size()-2);
                lexical_tokens = {tokens[0], tokens[1], exp, tokens[tokens.size()-2], tokens[tokens.size()-1]};
            }else if(tokens[tokens.size()-1] == ")"){
                string exp = mc.concacte(tokens, 2, tokens.size()-1);
                lexical_tokens = {tokens[0], tokens[1], exp, tokens[tokens.size()-1]};
            }
        }else if(tokens[0] == "else" || tokens[0] == "}"){
            lexical_tokens = tokens;
        }else if(regex_match(tokens[0], regex(mc.int_float_regex)) || regex_match(tokens[0], regex(mc.variable_regex))){
            if(tokens.size() <= 5){
                string logical_opertor = mc.concacte(tokens, 1, tokens.size()-2);
                lexical_tokens = {tokens[0], logical_opertor, tokens[tokens.size() - 2], tokens[tokens.size() - 1]};
            }else{
                string state = mc.concacte(tokens, 0, tokens.size()-1);
                lexical_tokens = {state, tokens[tokens.size()-1]};
            }
        }
    }else if(tokens.size() == 1){
        if(tokens[0] == "{" || tokens[0] == "}"){
            lexical_tokens = tokens;
        }
    }
    


    if(!lexical_tokens.empty()){
        // for(const auto& token: lexical_tokens){
        //     cout<<token<<endl;
        // }
        sa.parser(lexical_tokens, line_num, dataTypes, grammarSymbols);
        tokens.clear();
    }
}
