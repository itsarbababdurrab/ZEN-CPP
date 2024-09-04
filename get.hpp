
#ifndef GET_HPP
#define GET_HPP

#include <iostream>
#include <string>
#include <vector>
#include <variant>

using namespace std;


class Get{

    public:
        vector<string> tokenize(const string& content);
        variant<int, float, string, bool> get_user(int line_num, vector<string> tokens, string last_token); 

};



#endif //



