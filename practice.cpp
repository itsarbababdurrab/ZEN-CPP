#include "tinyexpr.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;


int main(){
    te_parser tep;
    string expression = "1==2 | 2==1";
    double res = tep.evaluate(expression);
    if(tep.success()){
        cout<<res<<endl;
    }
    return 0;
}
