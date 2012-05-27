#include "string_table.h"

string_table::string_table() {
    offset = 0;
}

int string_table::push_back(string s) {
    table.push_back(s);
    int t = offset;
    offset += s.length() + 1;//El null para asciiz
    return t;
}

int string_table::size() {
    return offset;
}

string string_table::to_string(string s) {
    string ss = "";
    for (unsigned int i = 0; i < s.length(); ++ i) {
        if ('\\' == s[i]) ss += '\\';
        ss += s[i];
    }
    return ss;
}

void string_table::dump() {

    fprintf(stdout, " STRINGS:\n");
    for (vector<string>::iterator it = table.begin(); it != table.end(); ++ it) {
        fprintf(stdout, "| %s\n", to_string(*it).c_str());
    }
    fprintf(stdout, "------------------------------------------------------\n\n");
}