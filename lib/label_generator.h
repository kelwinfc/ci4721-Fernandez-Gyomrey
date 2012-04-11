#ifndef __LABEL_GENERATOR
#define __LABEL_GENERATOR

#include <vector>
#include <cstdio>

#define ULLI unsigned long long int

using namespace std;

class label_generator{
    
    private:
        vector<ULLI> unused_labels;
        ULLI last;
    
    public:
        label_generator();
        
        ULLI next_label();
        
        void mark_unused(ULLI label);
};

#endif