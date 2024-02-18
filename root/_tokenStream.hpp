#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Position{
    string* fileName;
    unsigned index;
    unsigned row;
    unsigned column;
};
/*
    '@' is parsed as 'staticLine' operator
*/
enum TokenType{
    Whitespace, // space \t \n \r!
    Dimension, // 12px
    Number, //12.04
    HexColor, //#fff
    Group, //.userspace
    Tag, //#red
    Operator, //< >> <>
    BlockSpecifier, // " ' ( /*
    Text, // Unparsed text
    EscapedText, // Some escaped text like \a
    Variable, //  $test
    Reference, // &ref
    Sieve, // %sieve
    State, // :state
    Slot, // ::content
    Indentifier
};

struct Token{
    TokenType type;
    string content;
    Position pos;  
};


typedef vector<Token> Tokens;

class TokenStream{
    Tokens tokens;
    string input;

    public: 
        TokenStream(string _input){
            input = _input;
        }
};


class Tokenizer{
    ifstream fileHandle;
    unsigned fileSize;
    Position actualPosition;
    Tokens tokens;
    
    bool isNullString(string str){
        return str.size() == 0;
    }

    void consume(string str)
    {
        
    }
    
    public: 
        Tokenizer(string path){
            fileHandle.open(path);
            if(!fileHandle.good()){
                cerr << ("Błąd otwarcia pliku: " + path)<<endl;
                exit(-1);
            }
            fileHandle.seekg(0, ios::end);
            fileSize = fileHandle.tellg();
            actualPosition = {NULL, 0, 1, 1};
            fileHandle.seekg(0, ios::beg);
        }

        ~Tokenizer(){
            fileHandle.close();
        }

        void tokenize(){
            int c;
            while((c = fileHandle.get()) != EOF){
                cout<< (char) c << " " << actualPosition.row << ":" << actualPosition.column<<endl;
                
                // Follow position
                actualPosition.index++;
                actualPosition.column++;
                if(c == '\n'){
                    actualPosition.row++;
                    actualPosition.column = 1;
                }
            }
        }    
};
