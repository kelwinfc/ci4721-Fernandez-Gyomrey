#ifndef UTILS_H
#define UTILS_H

#define TYPE int
#define UNDEFINED 0
#define INVALID   1
#define NONE      2
#define INT       3
#define FLOAT     4
#define CHAR      5
#define BOOLEAN   6
#define STRING    7
#define PRINT_TYPE(t) (0 == t ? "UNDEFINED" : \
                      (1 == t ? "INVALID" : \
                      (2 == t ? "NONE" : \
                      (3 == t ? "INT" : \
                      (4 == t ? "FLOAT" : \
                      (5 == t ? "CHAR" : \
                      (6 == t ? "BOOLEAN" : \
                      (7 == t ? "STRING" : "UNKNOWN"))))))))

int lcm(int a, int b);

#endif