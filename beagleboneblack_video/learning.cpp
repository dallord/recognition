#include "learning.h"


X find_ref_point(vector<int> line){
    X u;
    int max = 8000;
    int x = 0, y = 0, w = 80;

    for (int i = 0; i < line.size(); i++){
        if (line[i] > max){
            max = line[i];
            x = i%w;
            y = i/w;
        }
    }
    if (max != 8000){
        u.x = x;
        u.y = y;
    }
    else {
        u.x = -1;
        u.y = -1;
    }

    return u;
}

void init_classes(){

    //"no object"
    classes[0].s0 = 0.;
    classes[0].s1 = 0.;
    classes[0].s2 = 0.;
    classes[0].type = 0;

    //"standing"
    classes[1].s0 = 1.;
    classes[1].s1 = 0.;
    classes[1].s2 = 0.;
    classes[1].type = 1;

    //"sitting"
    classes[2].s0 = 0.;
    classes[2].s1 = 1.;
    classes[2].s2 = 0.;
    classes[2].type = 2;

    //"lying"
    classes[3].s0 = 0.;
    classes[3].s1 = 0.;
    classes[3].s2 = 1.;
    classes[3].type = 3;
}

void set_type(Y y){
    if ((y.s0 = 0) && (y.s1 = 0) && (y.s2 = 0)) y.type = 0;
    if ((y.s0 = 1) && (y.s1 = 0) && (y.s2 = 0)) y.type = 1;
    if ((y.s0 = 0) && (y.s1 = 1) && (y.s2 = 0)) y.type = 2;
    if ((y.s0 = 0) && (y.s1 = 0) && (y.s2 = 1)) y.type = 3;
}

float rho(X u, X x_i){
    return sqrt((u.x - x_i.x)*(u.x - x_i.x) + (u.y - x_i.y)*(u.y - x_i.y));
}

float algorithm(X u){
    int argmax = 0; //will return argument of maximum value
    float max = 0; //maximum value
    int b; //true or false
    for (int e = 0; e < K; e++){
        float sum = 0;
        for (int i = 0; i < counter_train; i++){
            if (    (training_out[i].s0 == classes[e].s0) &&
                    (training_out[i].s1 == classes[e].s1) &&
                    (training_out[i].s2 == classes[e].s2))
                b = 1;
            else b = 0;
            sum += b*gam[i]*(h/rho(u, training_in[i]));
        }
        if (sum > max){
            max = sum;
            argmax = e;
        }
    }
    return argmax;
}

void init_gamma(){
    for (int i = 0; i < counter_train; i++){
        gam[i] = 0;
        if (algorithm(training_in[i]) != training_out[i].type)
            gam[i] += 1;
    }
}
