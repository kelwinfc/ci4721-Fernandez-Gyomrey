#include "string_table.h"

string_table::string_table() {
    offset = 0;
    push_back("Se trató de utilizar una propiedad no activa de la unión en (");
    push_back(", ");
    push_back(")");
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

// TODO escapar caracteres no escapados
string string_table::to_string(string s) {
    string ss = "";
    for (unsigned int i = 0; i < s.length(); ++ i) {
        if ('\\' == s[i]) ss += '\\';
        ss += s[i];
    }
    return ss;
}

void string_table::dump(ostream &strm) {

    strm << " STRINGS:" << endl;
    for (vector<string>::iterator it = table.begin(); it != table.end(); ++ it) {
        strm << "- \"" << to_string(*it) << "\"" << endl;
    }
}