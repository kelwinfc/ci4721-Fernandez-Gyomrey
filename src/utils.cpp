#include "utils.h"
#include <iostream>
#include <cmath>

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
