#ifndef _LLOG
#define _LLOG
#include <string>
#include <stdlib.h>

using namespace std;

class llog{

    private:

        bool registered_error;

    public:

        static const size_t ERR_LEN = 1000;

        llog();

        void warning(int l, int c, const char* e=0);
        
        void error(int l, int c, const char* e=0);

        void critical(const char* m);
        
        void failure(const char* c);
        
        void success();

        bool exists_registered_error();
};

#endif