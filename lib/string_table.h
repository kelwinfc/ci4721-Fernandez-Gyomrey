#ifndef STRING_TABLE_H
#define STRING_TABLE_H

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