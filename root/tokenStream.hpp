#pragma once
#include <iostream>
#include <vector>

#include "inputStream.hpp"
using namespace std;

#define UTF8_MASK -128

typedef unsigned unbooled;
typedef bool (*Predicate)(char);

class Node;

struct Token{
    string type;
    string value;
    Position start;

    operator Node () const;
};

typedef vector<Token> Tokens;
const string HEX_PATTERN = "/^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/";


Predicate nottedPredicate = NULL;

bool _not(char c){
    return !nottedPredicate(c);
}

Predicate notWrapper(Predicate predicate){
    nottedPredicate = predicate;
    return &_not;
}

char predictedChar = 0;

bool _isChar(char c){
    return c == predictedChar;
}

Predicate isCharWrapper(char c){
    predictedChar = c;
    return &_isChar;
}

bool isWhitespace(char c){
    switch(c){
        case '\t':
        case '\r':
        case '\n':
        case ' ':
            return true;
        default:
            return false;
    }
}

bool isNewline(char c){
    return c == '\n';
}

bool isOperator(char c){
    switch(c){
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
        case '&':
        case '|':
        case '!':
        case '~':
        case '>':
        case '<':
        case '^':
        case '?':
            return true;
        default:
            return false;
    }
}

bool isOperatorRepeatable(char c){
    switch(c){
        case '&':
        case '|':
        case '=':
        case '?':
        case '>':
        case '<':
            return true;
        default:
            return false;
    }
}

bool isPunctuation(char c){
    switch(c){
        case ',':
        case ';':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ':':
        case '#':
        case '.':
            return true;
        default:
            return false;
    }
}

bool isDigit(char c){
    return (c > 47 && c < 58);
}

/* moved from body */

bool hasPoint;

bool numberArrowFunction(char c){
    if(c == '.'){
        if(hasPoint) return false;
        hasPoint = true;
        return true;
    }
    return isDigit(c);
}

/* end of moved from body */

bool isHexDigit(char c){
    return (c > 47 && c < 58) || (c > 64 && c < 71) || (c > 96 && c < 103); 
}

bool isCommentStart(InputStream &input){
    return (input.peek() == '/' && (input.peek(1) == '/' || input.peek(1) == '*'));
}

bool isIdentStart(char c){
    // Allow utf8
    return (c == '_' || (c < 91 && c > 64) || (c < 123 && c > 96) || (c & UTF8_MASK));
}

bool isIdent(char c){
    return isIdentStart(c) || isDigit(c) || c == '-';
}

bool isNumberStart(InputStream &input){
    return isDigit(input.peek()) || (input.peek() == '.' && isDigit(input.peek(1)));
}

/*
    In out case we allow:
    rgb - 3hex
    rgba - 4hex
    rgb - 6hex
    rgba - 8hex
*/
unbooled isHex(InputStream &input){
    char hex = input.peek();
    unsigned len = 0;
    if(hex == '#'){
        while(++len < 9){
            if(!isHexDigit(input.peek(len))){
                break;
            }
        }
        switch(len){
            case 3:
            case 4:
            case 6:
            case 8:
                return len;
            default:
                return false;
        }
    }
    return false;
}

class TokenStream{
    Tokens tokens = {};
    InputStream& input;
    public:
        TokenStream(InputStream& input): input(input) {};

        Token createToken(string type, string value, Position start){
            return Token{
                type,
                value,
                start
            };
        }
        
        /**
         * To do
        */
        Token peek(unsigned offset = 0){
            if(tokens.size() == 0){
               const Token token = readNext();
               if(token.type != "") tokens.push_back(token);
            }
            if (offset < tokens.size()) return tokens[offset];
            while(tokens.size() <= offset){
                const Token token = readNext();
                if(token.type != "") tokens.push_back(token);
                else break;
            }
            if (offset < tokens.size()) return tokens[offset];
            else return Token{""};
        }

        Token next(){
            if(tokens.size() == 0)
                return readNext();
            Token token = tokens[0];
            tokens.erase(tokens.begin());
            
            return token;
        }

        void returnToken(Token token){
            tokens.insert(tokens.begin(), token);
        }

        bool eof(){
            return peek().type == "";
        }

        void err(string msg){
            input.err(msg);
        }

        Token readNext(){
            if(input.eof()) return Token{""};
            const char c = input.peek();
            // Whitespace
            if(isWhitespace(c)){
                return readWhitespace();
            }
            // Comments
            if(isCommentStart(input)){
                return readComment();
            }
            // Number
            if(isNumberStart(input)){
                return readNumber();
            }
            // Hex
            unbooled hexLength = isHex(input);
            if(hexLength){
                return readHex(hexLength);
            }
            // Punctutation
            if(isPunctuation(c)){
                return readPunctuation();
            }
            // Identifier
            if(isIdentStart(c)){
                return readIdent();
            }
            // Operator
            if(isOperator(c)){
                return readOperator();
            }
            // String
            if(c == '"' || c == '\''){
                return readString(c);
            }
            // // @ keyword
            // if(c == '@'){
            //     return readAtkeyword();
            // }
            // Variable
            if(c == '$'){
                return readVariable();
            }
            err("Can't handle character: " + c);
            return Token{};
        }

        string readWhile(Predicate predicate){
            string s = "";
            while(!input.eof() && predicate(input.peek())){
                s += input.next();
            }
            return s;
        }


        // Add support for 
        string readEscaped(char end){
            bool escaped = false;
            string str = "";
            input.next();
            while(!input.eof()){
                const char c = input.next();
                if(escaped){
                    escaped = false;
                }else if(c == '\\'){
                    escaped = true;
                }else if(c == end){
                    break;
                }
                str += c;
            }
            return str;
        }

        Token readWhitespace(){
            Position start = input.position();
            string value = readWhile(&isWhitespace);
            return createToken("space", value, start);
        }

        Token readComment(){
            Position start = input.position();
            input.next();
            switch(input.next()){
                case '/':
                    return readCommentSingle(start);
                case '*':
                    return readCommentMulti(start);
                default:
                    err("Error, bad comment");
                    return Token{};
            }
        }

        Token readCommentSingle(Position start){
            string value = readWhile(notWrapper(isNewline));
            return createToken("comment_singleline", value, start);
        }

        Token readCommentMulti(Position start){
            char prev = UTF8_MASK;
            string value = "";
            while(!input.eof()){
                const char next = input.next();
                if(next == '/' && prev == '*') break;
                value += prev;
                prev = next;
            }
            return createToken("comment_multiline", value, start);
        }

        Token readPunctuation(){
            const Position start = input.position();
            const string value = {input.next()};
            return createToken("punctuation", value, start);
        }

        Token readOperator(){
            const Position start = input.position();
            const char c = input.peek();
            string value = isOperatorRepeatable(c)
                ? readWhile(isOperatorRepeatable) : string(1, input.next());
            return createToken("operator", value, start);
        }

        Token readIdent(){
            const Position start = input.position();
            const string value = readWhile(&isIdent);
            return createToken("identifier", value, start);
        }

        Token readString(const char c){
            const Position start = input.position();
            const string value = readEscaped(c);
            string type = "string";
            if(c == '"') type = "string_double";
            if(c == '\'') type = "string_single";
            return createToken(type, value, start);
        }
        
        Token readNumber(){
            const Position start = input.position();
            hasPoint = false;
            string value = readWhile(&numberArrowFunction);
            return createToken("number", value, start);
        }

        Token readHex(unsigned length){
            const Position start = input.position();
            input.next();
            string value = "";
            for(unsigned i = 0; i < length; i++){
                value += input.next();
            }

            return createToken("color_hex", value, start);
        }

        // Token readAtkeyword(){
        //     const Position start = input.position();
        //     input.next();
        //     const string value = readWhile(&isIdent);
        //     return createToken("atkeyword", value, start);
        // }

        Token readVariable(){
            const Position start = input.position();
            input.next();
            const string value = readWhile(&isIdent);
            return createToken("variable", value, start);
        }

        // Token readReference(){
        //     const Position start = input.position();
        //     input.next();
        //     const string value = readWhile(&isIdent);
        //     createToken("variable", value, start);
        // }


};