
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <variant>
#include "main.hpp"
#include "syntaxAnalyzer.hpp"
#include "lexer.hpp"
#include "expressionHandler.hpp"
#include "semanticAnalyzer.hpp"
#include "errorHandler.hpp"
using namespace std;

void SyntaxAnalyzer::parser(vector<string>& tokens, int line_num, vector<string>& dataTypes, vector<string>& grammarSymbols){

    MainClass mc;
    ExpressionHandler exp_h;
    SemanticAnalyzer sa;
    ErrorHandler eh;
    vector<string> parsed_tokens;
    
    if(tokens.size() > 1 || (tokens.size() == 1 && tokens[0] == "else")){
       
        // the parser will analyze the syntax for multiple possibilities
        if(find(mc.dataTypes.begin(), mc.dataTypes.end(), mc.trim(tokens[0])) != mc.dataTypes.end()){
            
            if(regex_match(tokens[1], regex(mc.variable_regex))){
                
                if(tokens[0] == "Num" || tokens[0] == "Decimal"){
                    if(tokens[2] == "="){
                        if(!tokens.back().empty()){
                            parsed_tokens = tokens;
                        }else{
                            // throw error/////////////////////////////////////////////////////////////////
                            eh.handleError(line_num, "Missing Value", "Value Error", tokens);
                        }   
                    }else{
                        // throw error/////////////////////////////////////////////////////////////////
                        eh.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                    }
                }else{
                    parsed_tokens = tokens;
                }
            }else{
                // throw error missing var ////////////////////////////////////////////////////////////////
                eh.handleError(line_num, "Invalid Identifier", "Syntax Error", tokens);
            }
        }else if(find(mc.grammarSymbols.begin(), mc.grammarSymbols.end(), tokens[0]) != mc.grammarSymbols.end()){
            if(tokens[0] == "Set"){
                // Set>"string..."
                if(tokens[1] == ">"){
                    parsed_tokens = tokens;
                }else{
                    // throw error /////////////////////////////////////////////////////////////////
                    eh.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                }
            }
        }else if(tokens[0] == "if"){
            if(tokens.back() == "{"){
                if(tokens[1] == "("){
                    if(tokens[tokens.size()-2] == ")"){
                        parsed_tokens = tokens;
                    }else{
                        // throw error //////////////////////////////////////////////////////////////
                        eh.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                    }
                }else{
                    // throw error ////////////////////////////////////////////////////////////////
                    eh.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                }
            }else if(tokens.back() == ")"){
                if(tokens[1] == "("){
                    parsed_tokens = tokens;
                }else{
                    // throw error ///////////////////////////////////////////////////////////////
                    eh.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                }
            }
        }else if(tokens[0] == "else" || tokens[0] == "}"){
            if(tokens[0] == "else"){
                parsed_tokens = tokens;
            }else if(tokens[0] == "}"){
                if(tokens[1] == "else"){
                    parsed_tokens = tokens;
                }else{
                    // throw error
                    eh.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
                }
            }
        }else if(tokens.back() == "?"){
            if(tokens.size() == 2){
                parsed_tokens = tokens;
            }else if(tokens.size() > 2){
                if(regex_match(tokens[tokens.size()-2], regex(mc.int_float_regex)) || regex_match(tokens[tokens.size()-2], regex(mc.variable_regex))){
                    if(tokens.back() == "?"){
                        parsed_tokens = tokens;
                    }else{
                        // throw error /////////////////////////////////////////////////
                        eh.handleError(line_num, "Unexpected Syntax", "Syntax Error", tokens);
                    }
                }else{
                    // throw error ////////////////////////////////////////////////////
                    eh.handleError(line_num, "Unexpected Values", "Value Error", tokens);
                }
            }
        }

    }else if(tokens.size() == 1){
        if(tokens[0] == "{" || tokens[0] == "}"){
            parsed_tokens = tokens;
        }else {
            // throw error ///////////////////////////////////////////////////////////////////////
            eh.handleError(line_num, "Invalid Syntax", "Syntax Error", tokens);
        }
    }

    // Lexer lxr;
    // for(size_t i =0; i< parsed_tokens.size(); ++i){
        // lxr.prnt(parsed_tokens[i]);
    // }

    if(!parsed_tokens.empty()){
        sa.semantic_analyzer(parsed_tokens, line_num);
    }
}
