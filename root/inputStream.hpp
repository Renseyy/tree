#pragma once
#include <iostream>
using namespace std;

struct Position{
    unsigned cursor;
    unsigned line;
    unsigned column;
};

class InputStream{
    string input;
    unsigned cursor = 0;
    unsigned line = 1;
    unsigned column = 0;
    unsigned previousLineLength = 0;
    unsigned inputLength;

    public:
        InputStream(string input): input(input) {
            inputLength = input.size();
        }

        unsigned getLineLengthForOffset(unsigned offset){
            return offset - input.find_last_of('\n');
        }

        Position position(){
            return Position{cursor, line, column};
        }

        char peek(unsigned offset = 0){
            return input[cursor + offset];
        }

        char next(){
            const char c = input[cursor++];
            if(c == '\n'){
                previousLineLength = column;
                line++;
                column = 0;
            }else{
                column++;
            }
            return c;
        }

        void back(){
            cursor--;
            if(column == 0){
                line--;
                column = getLineLengthForOffset(cursor);
            }
        }

        bool eof(){
            return cursor >= inputLength;
        }

        void err(string msg){
            cerr << msg << " (" << line << ":" << column << ")"<<endl;
            exit(-1);
        }
};