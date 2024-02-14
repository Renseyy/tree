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

struct Token{
    string type;
    string content;
    Position pos;  
};

class Tokenizer{
    ifstream fileHandle;
    unsigned fileSize;
    Position actualPosition;
    
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
