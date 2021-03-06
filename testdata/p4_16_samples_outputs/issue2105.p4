#include <core.p4>
#define V1MODEL_VERSION 20180101
#include <v1model.p4>

header H {
    bit<8> a;
}

struct Headers {
    H h;
}

control c() {
    apply {
        bit<8> x = 0;
        bit<8> y = 0;
        y = (x < 4 ? 8w2 : 8w1)[7:0] | 8w8;
    }
}

control e<T>();
package top<T>(e<T> e);
top(c()) main;

