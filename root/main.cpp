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
#include "generator.hpp"
using namespace std;


void replaceAll(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void printNode(Node& node, unsigned level){
    cout << string(level, ' ')  << node.type;
    if(node.content != ""){
        string content = node.content;
        replaceAll(content, "\n", "⏎");
        replaceAll(content, " ", "·");
        cout << ": " << "\"" << content << "\""<<endl;
    }else{
        cout << ": [" << node.children.size() << "]"<<endl;
        for(auto& child : node.children){
            printNode(child, level + 1);
        }
    }
}

int main(int argc, char** argv){
    // Xor is not fully implemented
    if(argc < 2){
        cerr<<"Usage: "<< argv[0] << " file.tree"<<endl; 
        exit(-1);
    }
    ifstream fileHandle(argv[1]);
    if(!fileHandle.good()){
        cerr << ("Błąd otwarcia pliku: " + string(argv[1]))<<endl;
        exit(-1);
    }
    ostringstream oss;
    oss << fileHandle.rdbuf();
    InputStream is(oss.str());
    TokenStream ts(is);
    Parser parser(ts);
    Node script = parser.parseScript();
    printNode(script, 0);
    generateIntoPath("./test.js", script);
}