#ifndef _LLOG
#define _LLOG
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

class llog{

    private:

        map<int, map<int, vector<string> > > messages;

        bool registered_error;

    public:

        stringstream buf;

        llog();

        void warning(int l, int c);
        
        void error(int l, int c);

        bool exists_registered_error();

        void dump(ostream &errd);
};

#endif