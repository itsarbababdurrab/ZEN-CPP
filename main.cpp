
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "main.hpp"
#include "lexer.hpp"
using namespace std;



void MainClass::readFile(string& file_name){
    MainClass mc;
    ifstream file(file_name);
    if(file.is_open()){
        string line;
        int line_num = 0;
        // create an object for Lexer class declared in the Lexer.h(header file) and call the function.
        Lexer lexer;
        while(getline(file, line)){
            // output lines
            line_num+=1;
            if(line.find_first_not_of(" \t") != string::npos){
                lexer.lexicalAnalyzer(line ,line_num , mc.dataTypes , mc.grammarSymbols);
            }else{
                
            }
        }
        // close the fille
        file.close();
    }else{
        cerr<< "Unable to open the file"<<endl;
    }
}


int main(){
    MainClass mc;
    // Read file path
    string file_name = "./program.txt";
    mc.readFile(file_name);
    return 0;
}