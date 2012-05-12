#ifndef _INST
#define _INST

int newtemp();

class inst{

};

class quad : public inst{
    public:
        enum OP {ADD, CALL, PARAM, ACCESS, REF, DEREF};

    private:
        OP op;
        int arg0;
        int arg1;
        int arg2;

    public:
        quad(OP op, int arg0, int arg1 = -1, int arg2 = -1);
};

class mi : public inst{

};

#endif