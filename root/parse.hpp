#pragma once
#include <iostream>
#include <vector>

#include "tokenStream.hpp"

using namespace std;

struct Node;

typedef vector<Node> Nodes;
typedef vector<string> strings;

class Parser;

typedef bool (Parser::*TypePredicate)(strings); 

typedef bool (Parser::*ExpressionPredicate)(void**);

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

    for(auto iterator = v.cbegin() + index + deleteCount; iterator < v.cend(); iterator++){
        vec.push_back(v[iterator]);
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
    for(auto& i : v){
        bool uniq = true;
    }
}

class Parser{
    TokenStream tokens;

    private:
        Node createTextNode(string type, string content, Position start){
            return Node{type, content, {}, start};
        }

        Node createContainerNode(string type, Nodes children, Position start){
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
            return isType("space");
        }

        bool isComment(){
            return isType("comment");
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
                tokens.err("Expecting " + type + ": " + joinStrings(chars));
                return TokenPair{};
            }
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
                    Node b = createTextNode("punctuation", ":", a.start);
                    Nodes nodes = concat({b}, a.children);
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
                    const Node propertyNode = createContainerNode(
                        "property", properties, properties[0].start);
                    Nodes value_ = drop(values, puncIndex + 1);
                    if (punctuation == '{') {
                        Node block = Node{}; //parseBlock();
                        value_.push_back(block);
                    }
                    Node valueNode = createContainerNode(
                        "value", value_, value_[0].start);
                    Nodes declarationValue = {propertyNode, values[puncIndex], valueNode};
                    if (punctuation == ';') {
                        const Node start = skipPunctuation({";"}).start;
                        declarationValue.push_back(start);
                    }
                    return createContainerNode(
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
                        return maybeDeclaration('{', values) || parseRule(values);
                    }else{
                        return concat(values, parseBlock());
                    }
                }
                if(isPunctuation({";"})){
                    return {maybeDeclaration(';', values)};
                }
            }
            return values;
        }

        Nodes parseExpression(ExpressionPredicate predicate, void** data){
            Nodes value;
            Nodes declaration;
            while(true){
                if(tokens.eof() || !(this->*predicate)(data)) break;
                // Declaration
                if(isPunctuation({":"}) && declaration.size()){
                    value.push_back(parseDeclaration(declaration));
                }
            }
        }

        Node parseAtom(){
            // Changed to linear instad of array function
            Node result;
            if(isPunctuation({"("})){
                result = parseWrapped('parentheses', '(', ')')
            }
            // Interpolation
            if (this.is_interpolation()) {
                result = this.parse_interolation()
            }

            // Interpolation
            if (this.is_attribute()) {
                result = this.parse_attribute()
            }

            // Attr
            if (this.is_punctuation('[')) {
                result = this.parse_wrapped('query', '[', ']')
            }
            // Class
            if (this.is_punctuation('.')) {
                result = parseSelector('group', '.')
            }
            // Id
            if (this.is_punctuation('#')) {
                result = parseSelector('id', '#')
            }
            // Pseudo Element
            if (this.is_punctuation('::')) {
                result = parseSelector('pseudo_element', ':')
            }
            // Pseudo Class
            if (this.is_punctuation(':')) {
                const next = this.tokens.peek(1)
                if (
                (next.type === 'identifier') ||
                (next.type === 'punctuation' && next.value === '#')
                ) {
                return this.parse_selector('pseudo_class', ':')
                }
            }
            // Token

            /// Check is part of function
            return tokens.next();
        }

        // parseDeclaration

        Node parseWrapped(string type, string open, string close, Position _start){
            Node start = skipPunctuation({open}).start;

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
            return createContainerNode("script", nodes, Position{});
        }


};