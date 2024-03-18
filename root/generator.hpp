#include <fstream>
#include <iostream>
#include "parse.hpp"

using namespace std;

string generateFromNode(Node&);

string generateFromRule(Node& rule){
    string name = rule.children[0].children[0].content;
    string result = "MEM['" + name + "'] = ";
    return result;
}

string generateFromScript(Node& script){
    // Wrap JS into annonymus async function
    string result = "/* AUTO GENERATED FROM TREE'lang */\n(async() => {\n\tconst MEM = {}\n";
    for(auto& node : script.children){
        result += "\t" + generateFromNode(node) + "\n";
    }
    return result + "})()";
}

string generateFromNode(Node& node){
    if(node.type == "script"){
        return generateFromScript(node);
    }else if(node.type == "space" || node.type == "endline"){
        return "/* S||NL */";
    }else if(node.type == "rule"){
        return generateFromRule(node);
    }
    return "console.error('Unsuported node type: " + node.type + "');";
}


void generateIntoPath(string path, Node& root){
    ofstream script(path);
    script << generateFromNode(root);
}