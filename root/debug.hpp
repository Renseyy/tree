#pragma once
#include <iostream>
using namespace std;
class DepthCounter
{
    static int depth;

public:
    DepthCounter(const string& name)
    {
        cout <<"\e[35m[ " << depth << " ] \e[0m" << string(depth*2, ' ') << name << "\n";
        ++depth;
    }

    ~DepthCounter()
    {
        --depth;
    }
};

int DepthCounter::depth = 0;