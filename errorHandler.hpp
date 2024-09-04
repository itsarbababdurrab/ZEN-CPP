#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Error:public runtime_error{
    private:
    static string concate(vector<string> tokens){
        string result;
        for(const auto& token: tokens){
            if(!result.empty()){
                result+= " ";
            }
            result+=token;
        }
        return result;
    }
    public:
        Error(string type, string desc, int line_num, vector<string> content): runtime_error(type +": "+ desc +" in Line "+ to_string(line_num) +": "+ concate(content)){};
};


class ErrorHandler{
    public:
        void handleError(int line_num, string desc, string type, vector<string> content);

};

#endif 
