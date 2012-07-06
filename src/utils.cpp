#include "utils.h"

using namespace std;

int gcd(int a, int b){
    if ( b == 0 ){
        return a;
    }
    return gcd(b, a%b);
}

int lcm(int a, int b){
    if ( a == 0 && b == 0 ){
        return 1; // lcm se utiliza para obtener la alineacion comun de un
                  // conjunto de datos, si todos tienen tamaño cero la
                  // alineacion es de cualquier byte
    }
    return abs((double)a*b)/gcd(a,b);
}

char* itoa_(int i){
    char* s = new char[12];
    int k = 12;
    s[--k] = 0;
    while (i > 0) {
        s[--k] = '0' + (i % 10);
        i /= 10;
    }
    if (0 < k) {
        for (int i = 0; i + k < 12; ++ i) {
            s[i] = s[i + k];
        }
    }
    return s;
}