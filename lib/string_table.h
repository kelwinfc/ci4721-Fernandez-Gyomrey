#ifndef __STRING_TABLE
#define __STRING_TABLE

#include<string>
#include<vector>
#include<iostream>

using namespace std;

class string_table{
private:

    int offset;

    vector<string> table;

public:

    string_table();

    int push_back(string s);

    int size();

    void dump(ostream &strm);

    static string to_string(string s);
};

#endif