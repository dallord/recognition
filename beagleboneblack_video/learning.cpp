#include "learning.h"

int counter_train = 0; //number of pairs in the training set - l

//training pairs
X* training_in; //list of training vectors - x_i
Y* training_out; //list of training answers - y_i

X learning_in; //learning point
Y classes[K];

float* gam; //array of gamma coefficients
float h; //width of the window

X find_ref_points(vector<int> line){

    vector<Point> object;
    Point p;

    X u;
    int max = 8000;
    int x = 0, y = 0, w = 80;

    int t_x = 0;
    int t_y = 0;
    int b = 0;

    int d = 0;
    int m_dist = 0;

    //second point
    for (int i = 0; i < line.size(); i++){
        if (line[i] > max){
            p.x = i%w;
            p.y = i/w;
            p.weight = line[i];
            object.push_back(p);
        }
    }

    for (int i = 0; i < object.size(); i++){
        t_x += object[i].weight*object[i].x;
        t_y += object[i].weight*object[i].y;
        b += object[i].weight;
    }
    if (b != 0){
        u.x1 = t_x/b;
        u.y1 = t_y/b;
    } else {
        u.x1 = -1;
        u.y1 = -1;
    }

    //first point
    for (int i = 0; i < line.size(); i++){
        if (line[i] > max){
            max = line[i];
            x = i%w;
            y = i/w;
        }
    }
    if (max != 8000){
        u.x0 = x;
        u.y0 = y;
        //third point
        for (int e = 0; e < object.size(); e++) {
            d = sqrt((float)((object[e].x - u.x0)*(object[e].x - u.x0) + (object[e].y - u.y0)*(object[e].y - u.y0)));
            if (d > m_dist){
                m_dist = d;
                u.x2 = object[e].x;
                u.y2 = object[e].y;
            }
        }

    }
    else {
        u.x0 = -1;
        u.y0 = -1;
        u.x2 = -1;
        u.y2 = -1;
    }

    object.clear();

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
    return sqrt((u.x0 - x_i.x0)*(u.x0 - x_i.x0) + (u.y0 - x_i.y0)*(u.y0 - x_i.y0)
                + (u.x1 - x_i.x1)*(u.x1 - x_i.x1) + (u.y1 - x_i.y1)*(u.y1 - x_i.y1)
                + (u.x2 - x_i.x2)*(u.x2 - x_i.x2) + (u.y2 - x_i.y2)*(u.y2 - x_i.y2));
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
