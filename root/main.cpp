//This file must load files we want to transpile, additionaly must be able to load definition files but this later
#include <iostream>
#include "utils.hpp"
#include "tokenizer.hpp"
using namespace std;

int main(int argc, char** argv){
    if(argc < 2){
        cout<<"Usage: "<< argv[0] << " file.tree"<<endl; 
    }
    for(unsigned i = 1; i < argc; i++){
        Tokenizer tokenizer(string(argv[i]));
        tokenizer.tokenize();
    }
    
}