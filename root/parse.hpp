
/* Typy lini
    let $a = 12 // StaticDeclaration
*/


#pragma once
#include <iostream>
#include <vector>
#include "debug.hpp"

#include "tokenStream.hpp"

using namespace std;

struct Node;

typedef vector<Node> Nodes;
typedef vector<string> strings;

class Parser;

typedef bool (Parser::*TypePredicate)(strings);

typedef bool (Parser::*ExpressionPredicate)(void*);

struct Node{
    string type;
    string content;
    Nodes children;
    Position start;

    // Implement falsy Node
    operator bool() const{
        return type != "";
    }

    bool operator == (const Node &n){ 
        if (type == n.type && content == n.content && children == n.children) 
            return true; 
        return false; 
    } 
};

//Add auto casting from Token
Token::operator Node () const{
    return Node{
        type,
        value,
        {},
        start
    };
}

struct TokenPair{
    Token start;
    Token next;
};

string joinStrings(strings values){
    string result;
    for(auto& value: values){
        result += value;
    }
    return result;
}


// Lodash
template<typename T>
vector<T> splice(vector<T> const &v, unsigned index, unsigned deleteCount = 0, vector<T> elementsToAdd = {})
{
    // Add start
    vector<T> vec(v.cbegin(), v.cbegin() + index);
    
    //Add elements
    for(auto& element : elementsToAdd){
        vec.push_back(element);
    }
    
    //Add tail

    for(auto iterator = v.cbegin() + index + deleteCount; iterator != v.cend(); iterator++){
        vec.push_back(*iterator);
    }
    return vec;
}

template<typename T>
vector<T> take(vector<T> const &v, int n = 1)
{
    auto first = v.cbegin();
    auto last = v.cbegin() + n;
 
    vector<T> vec(first, last);
    return vec;
}

template<typename T>
vector<T> concat(vector<T> const& v, vector<T>& toAdd)
{
    vector<T> vec = v;
    for(auto& add : toAdd){
        vec.push_back(add);
    }
    return vec;
}

template<typename T>
vector<T> concat(vector<T> const& v, T& toAdd)
{
    vector<T> vec = v;
    vec.push_back(toAdd);
    return vec;
}

template<typename T>
vector<T> drop(vector<T> const &v, int n)
{
    auto first = v.cbegin() + n;
    auto last = v.cend();
 
    vector<T> vec(first, last);
    return vec;
}

template<typename T>
vector<T> vectorXor(vector<T> const &v, vector<T> const &u)
{
    vector<T> vec;
    // for(auto& i : v){
    //     bool uniq = true;
    //     for(auto& )
    // }
    return v;
}

string join(strings ss){
    string s;
    for(auto& _ : ss){
        s+= _ + " ";
    }
    return s;
}

class Parser{
    TokenStream tokens;

    private:
        Node createNode(string type, string content, Position start){
            
            return Node{type, content, {}, start};
        }

        Node createNode(string type, Nodes children, Position start){
            
            return Node{type, "", children, start};
        }

        // isType

        bool isType(string type, strings values = {}){
            
            Token t = tokens.peek();
            if(values.size() == 0) return t.type != "" ? t.type == type : false;
            // reduce
            bool a = true;
            unsigned i = 0;
            for(auto& c : values){
                Token t = tokens.peek(i);
                a = (t.type == "") ? false : ( a && t.type == type && t.value == c);
                if(!a) return false;
                i++;
            }
            return true;
        }

        bool isSpace(){
            
            return isType("space") or isType("endline");
        }

        bool isComment(){
            
            return isType("comment");
        }

        bool isEndline(){
            return isType("endline");
        }

        bool isPunctuation(strings values){
            
            return isType("punctuation", values);
        }

        bool isOperator(strings values){
            
            return isType("operators", values);
        }

        bool isIdentifier(strings values){
            
            return isType("identifier", values);
        }

        //isAtKeywoar ommited 
        bool isInterpolation(){
            
            return isPunctuation({"#", "{"});
        }

        bool isAttribute(){
            
            return isPunctuation({"#", "["});
        }   

        TokenPair skipType(string type, TypePredicate isType, strings chars){
            
            if((this->*isType)(chars)){
                return TokenPair{
                    tokens.peek(),
                    tokens.next()
                };
            }else{
                return TokenPair{};
            }
        }

        TokenPair skipNewline(){
            return skipType("newline", &Parser::isPunctuation, {});
        }

        TokenPair skipPunctuation(strings chars){
            
            return skipType("punctuation", &Parser::isPunctuation, chars);
        }

        TokenPair skipOperator(strings chars){
            
            return skipType("operator", &Parser::isOperator, chars);
        }

        //skipAtKeyword

        void unexpected(){
            
            Token t = tokens.peek();
            tokens.err("Unexpected token: " + t.type + " (" + t.value + ") ");
        }

        //parse stylesheet -> parseScript -> moved to Public
        int findIndexByTypeAndContent(Nodes& values, string type, string content = ""){
            
            for(int i = 0; i < values.size(); i++){
                if(values[i].type == type && (content == "" || content == values[i].content))
                    return i;
            }
            return -1;
        }

        Node maybeDeclaration(char punctuation, Nodes& values){
            
            bool expandedPseudo = false;
            // If the declaration ends with a ";" expand the first pseudo_class
            // because pseudo_class can't be part of a declaration property
            if(punctuation == ';'){
                int pseudoIndex = findIndexByTypeAndContent(values, "pseudo_class");
                if(pseudoIndex > 0){
                    Node a = values[pseudoIndex];
                    Node b = createNode("punctuation", ":", a.start);
                    Nodes nodes = concat((Nodes) {b}, a.children);
                    values = splice(values, pseudoIndex, 1, nodes);
                    expandedPseudo = true;
                }
            }
            int puncIndex = findIndexByTypeAndContent(values, "punctuation", ":");
            if(puncIndex >= 0){
                Node maybeSpace = values[puncIndex + 1];
                // If we found a space, it wasn't a pseudo class selector,
                // so parse it as a declaration
                // http://www.sassmeister.com/gist/0e60f53033a44b9e5d99362621143059
                if (maybeSpace.type == "space" || expandedPseudo) {
                    Position start = values[0].start;
                    Nodes properties = take(values, puncIndex);
                    const Node propertyNode = createNode(
                        "property", properties, properties[0].start);
                    Nodes value_ = drop(values, puncIndex + 1);
                    if (punctuation == '{') {
                        Node block = parseBlock();
                        value_.push_back(block);
                    }
                    Node valueNode = createNode(
                        "value", value_, value_[0].start);
                    Nodes declarationValue = {propertyNode, values[puncIndex], valueNode};
                    if (punctuation == ';') {
                        const Node start = skipPunctuation({";"}).start;
                        declarationValue.push_back(start);
                    }
                    return createNode(
                        "declaration", declarationValue, start);
                }
            }

            return Node{};
        }

        Nodes parseNode(){
            
            if(isSpace() || isComment()){
                Token t = tokens.next();
                return {Node{t.type, t.value, {}, t.start}};
            }
            Nodes values;
            while(!tokens.eof()){
                // AtRule is ommited
                values.push_back(parseAtom());
                if(isPunctuation({"{"})){
                    if(values.size() > 0){
                        // Warning, i dont know how this will be casted
                        Node dec = maybeDeclaration('{', values);
                        if(dec) return {dec};
                        return {parseRule(values)};
                    }else{
                        Node parsedBlock = parseBlock();
                        return concat(values, parsedBlock);
                    }
                }
                if(isPunctuation({";"})){
                    return {maybeDeclaration(';', values)};
                }
                if(isEndline()){
                    return {maybeDeclaration('\0', values)};
                }
            }
            return values;
        }

        Nodes parseExpression(ExpressionPredicate predicate, void* data){
            
            Nodes value;
            Nodes declaration;
            while(true){
                if(tokens.eof() || !(this->*predicate)(data)) break;
                // Declaration
                if(isPunctuation({":"}) && declaration.size()){
                    value.push_back(parseDeclaration(declaration));
                    // xor
                    value = vectorXor(value, declaration);
                    declaration = {};
                }
                // Atom
                if(tokens.eof() || !(this->*predicate)(data)) break;
                const Node atom = parseAtom();
                value.push_back(atom);
                if(atom.type != "space" && atom.type != "punctuation"){
                    declaration.push_back(atom);
                }
            }
            return value;
        }

        Node parseAtom(){
            
            // Changed to linear instad of array function
            Node result;
            // This is lazy elmulation that this is some sort of inner function
            do{ 
                if(isPunctuation({"("})){
                    result = parseWrapped("parentheses", "(", ")");
                    break;
                }
                // Interpolation
                if(isInterpolation()){
                    result = parseInterpolation();
                    break;
                }

                // Interpolation
                if(isAttribute()){
                    result = parseAttribute();
                    break;
                }

                // Attr
                if(isPunctuation({"["})){
                    result = parseWrapped("query", "[", "]");
                    break;
                }
                // Class
                if(isPunctuation({"."})) {
                    result = parseSelector("group", ".");
                    break;
                }
                // Id
                if(isPunctuation({"#"})) {
                    result = parseSelector("id", "#");
                    break;
                }
                // Pseudo Element
                if(isPunctuation({"::"})) {
                    result = parseSelector("pseudo_element", ":");
                    break;
                }
                // Pseudo Class
                if (isPunctuation({":"})) {
                    Node next = tokens.peek(1);
                    if (
                    (next.type == "identifier") ||
                    (next.type == "punctuation" && next.content == "#")
                    ) {
                        result = parseSelector("pseudo_class", ":");
                        break;
                    }
                }
                result = tokens.next();
            }while(false);
            // Token

            /// Check is part of function
            return wrapIntoFunction(result);
        }

        bool parseDeclarationArrow(void* _){
            
            Node* secondSeparator = (Node*) _;
            if(isPunctuation({";"})){
                *secondSeparator = tokens.next();
                return false;
            }
            if(isPunctuation({","})){
                *secondSeparator = tokens.next();
                return false;
            }
            if(isPunctuation({")"})) return false;
            return true;
        }

        Node parseDeclaration(Nodes property){
            
            Node firstSeparator = skipPunctuation({":"}).start;
            Node secondSeparator;
            Nodes value = parseExpression(&Parser::parseDeclarationArrow, &secondSeparator);
            Node propertyNode = createNode("property", property, property[0].start);
            Node valueNode = createNode("value", value, value[0].start);
            Nodes declarationValue = {propertyNode, firstSeparator, valueNode};
            if(secondSeparator) declarationValue.push_back(secondSeparator);
            return createNode("declaration", declarationValue, property[0].start);
        }
        
        bool parseWrappedArrow(void* data){
            
            return !isPunctuation({*(string*)data});
        }

        Node parseWrapped(string type, string open, string close, Position _start = {}){
            
            Node start = skipPunctuation({open}).start;
            Nodes value = parseExpression(&Parser::parseWrappedArrow, (void *) &close);
            Node next = skipPunctuation({close}).next;
            return createNode(type, value, start.start);
        }

        Node parseBlock(){
            Node start = skipPunctuation({"{"}).start;
            Nodes value;
            while(
                !tokens.eof() &&
                !isPunctuation({"}"})
            ){
                Nodes nodes = parseNode();
                for(auto node : nodes){
                    value.push_back(node);
                }
            }
            Node next = skipPunctuation({"}"}).next;
            if(isPunctuation({";"})){
               skipPunctuation({";"}); 
            }
            return createNode("block", value, start.start);
        }

        bool parseArgumentsArrow(void * _){
            
            if(isPunctuation({","})) return false;
            if(isPunctuation({")"})) return false;
            return true;
        }
        Node parseArguments(string type){
            
            Node start = skipPunctuation({"("}).start;
            Nodes value;
            if(type == "pseudo_class"){
                while(!tokens.eof() && !isPunctuation({")"})){
                    Nodes toAdd = parseExpression(&Parser::parseArgumentsArrow, NULL);
                    value = concat(value, toAdd);
                }
                if(isPunctuation({","})){
                    value.push_back(tokens.next());
                }
            }
            Node next = skipPunctuation({")"}).next;
            return createNode("arguments", value, start.start);
        }

        Node wrapIntoFunction(Node& node){
            
            if(isPunctuation({"("}) && 
                (node.type == "identifier") ||
                (node.type == "function") ||
                (node.type == "interpolation") ||
                (node.type == "pseudo_class") ||
                (node.type == "attribute")
            ){
                return parseFunction(node);
            }
            return node;
        }

        Node parseFunction(Node node){
            
            Node args = parseArguments(node.type);
            // Bez (Nodes) nie wiedział jaki to typ xD
            return createNode("function", (Nodes) {node, args}, node.start);
        }

        Node parseInterpolation(){
            
            Node start = skipPunctuation({"#"}).start;
            return parseWrapped("interpolation", "{", "}", start.start);
        }

        Node parseAttribute(){
            
            Node start = skipPunctuation({"#"}).start;
            return parseWrapped("interpolation", "[", "]", start.start);
        }

        //Parse AtRule ommited

        Node parseRule(Nodes selectors){
            
            Node selector = createNode("selector", selectors, selectors[0].start);
            Node block = parseBlock();
            return createNode("rule", (Nodes) {selector, block}, selector.start);
        }

        Node parseSelector(string type, string punctuation){
            
            Node start = skipPunctuation({punctuation}).start;
            if(isPunctuation({":"})){
                skipPunctuation({":"});
            }
            Nodes value;
            Node next = isInterpolation() ? parseInterpolation() : tokens.next();
            while(next.type == "identifier" || next.type == "interpolation" || next.type == "operator"){
                value.push_back(next);
                next = isInterpolation() ? parseInterpolation() : tokens.peek();
                if(!next) break;
                if(next.type == "identifier") tokens.next();
                if(next.type == "operator") tokens.next();
            }
            if(!value.size()){
                tokens.err("Selector (" + type + ") expected 'identifier' or 'interpolation'");
            }
            return createNode(type, value, start.start);
        }


    public:
        Parser(TokenStream tokens): tokens(tokens) {};

        Node parseScript(){
            
            Nodes nodes;
            while(!tokens.eof()){
                for(auto& node : parseNode()){
                    nodes.push_back(node);
                }
            }
            return createNode("script", nodes, Position{});
        }


};