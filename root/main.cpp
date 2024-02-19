#if 0  // -- build and run wrapper script for C++ ------------------------------
TMP=$(mktemp -d)
g++ -o ${TMP}/a.out ${0} && exec -a ${0} ${TMP}/a.out ${@:1}; RV=${?}
rm -rf ${TMP}
exit ${RV}
#endif

//This file must load files we want to transpile, additionaly must be able to load definition files but this later
#include <iostream>
#include <fstream>
#include <sstream>
#include "inputStream.hpp"
#include "tokenStream.hpp"
#include "parse.hpp"
using namespace std;

int main(int argc, char** argv){
    // if(argc < 2){
    //     cerr<<"Usage: "<< argv[0] << " file.tree"<<endl; 
    //     exit(-1);
    // }
    // ifstream fileHandle(argv[1]);
    // if(!fileHandle.good()){
    //     cerr << ("Błąd otwarcia pliku: " + string(argv[1]))<<endl;
    //     exit(-1);
    // }
    // ostringstream oss;
    // oss << fileHandle.rdbuf();
    // InputStream is(oss.str());
    // TokenStream ts(is);
    // Token t = ts.next();
    // // Null token has no type
    // while(t.type != ""){
    //     cout << t.type << " " << t.value << endl;
    //     t = ts.next();
    // }   

    strings a = {"a", "b", "c", "d"};
    strings b = slice(a, 2, 4);
    for(auto& c : b){
        cout << c <<endl;
    }
}