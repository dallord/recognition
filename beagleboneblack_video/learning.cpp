#include "learning.h"

int counter_train = 0; //number of pairs in the training set - l

//training pairs
//X* training_in; //list of training vectors - x_i
//Y* training_out; //list of training answers - y_i
Pair* training_set; //list of training pairs - x_i; y_i

X learning_in; //learning point
Y learning_out; //answers for learning point
Y classes[K];

float* gam; //array of gamma coefficients
float h; //width of the window

X find_ref_points(vector<int> line){

    vector<Point> object;
    Point p;

    X u;
    int max = ENV_MAX;
    int x = 0, y = 0, w = 80;

    int t_x = 0;
    int t_y = 0;
    int b = 0;

    int d = 0;
    int m_dist = 0;

    //second point
    for (int i = 0; i < line.size(); i++){
        if ((line[i] > max) && (line[i] < BODY_MAX)){
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
        if ((line[i] > max) && (line[i] < BODY_MAX)){
            max = line[i];
            x = i%w;
            y = i/w;
        }
    }
    if (max != ENV_MAX){
        u.x0 = x;
        u.y0 = y;
        //third point
        for (int e = 0; e < object.size(); e++) {
            d = sqrt((float)((object[e].x - u.x0)*(object[e].x - u.x0) +
                             (object[e].y - u.y0)*(object[e].y - u.y0)));
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
    classes[0].s3 = 0.;
    classes[0].type = 0;

    //"standing"
    classes[1].s0 = 1.;
    classes[1].s1 = 0.;
    classes[1].s2 = 0.;
    classes[1].s3 = 0.;
    classes[1].type = 1;

    //"sitting"
    classes[2].s0 = 0.;
    classes[2].s1 = 1.;
    classes[2].s2 = 0.;
    classes[2].s3 = 0.;
    classes[2].type = 2;

    //"lying on the bed"
    classes[3].s0 = 0.;
    classes[3].s1 = 0.;
    classes[3].s2 = 1.;
    classes[3].s3 = 0.;
    classes[3].type = 3;

    //"lying on the floor"
    classes[4].s0 = 0.;
    classes[4].s1 = 0.;
    classes[4].s2 = 0.;
    classes[4].s3 = 1.;
    classes[4].type = 4;
}

int set_type(Y y){
    int t = 0;
    if ((y.s0 == 0) && (y.s1 == 0) && (y.s2 == 0) && (y.s3 == 0)) t = 0;
    if ((y.s0 == 1) && (y.s1 == 0) && (y.s2 == 0) && (y.s3 == 0)) t = 1;
    if ((y.s0 == 0) && (y.s1 == 1) && (y.s2 == 0) && (y.s3 == 0)) t = 2;
    if ((y.s0 == 0) && (y.s1 == 0) && (y.s2 == 1) && (y.s3 == 0)) t = 3;
    if ((y.s0 == 0) && (y.s1 == 0) && (y.s2 == 0) && (y.s3 == 1)) t = 4;
    return t;
}

float rho(X u, X x_i){
    return sqrt((u.x0 - x_i.x0)*(u.x0 - x_i.x0) + (u.y0 - x_i.y0)*(u.y0 - x_i.y0)
                + (u.x1 - x_i.x1)*(u.x1 - x_i.x1) + (u.y1 - x_i.y1)*(u.y1 - x_i.y1)
                + (u.x2 - x_i.x2)*(u.x2 - x_i.x2) + (u.y2 - x_i.y2)*(u.y2 - x_i.y2));
}

float Gamma(X u, Y y){
    float sum = 0;
    float c = 0;
    for (int i = 0; i < counter_train; i++){
        if (training_set[i].training_out.type == y.type){
            sum += gam[i]*(h/(rho(u, training_set[i].training_in) + 1.));
        }
    }
    return sum;
}

Y algorithm(X u){
    Y result;
    result.s0 = 0;
    result.s1 = 0;
    result.s2 = 0;
    result.s3 = 0;

    int argmax = 0; //will return argument of maximum value
    float max = 0; //maximum value
    float sum = 0;

    for (int e = 0; e < K; e++){
        sum = Gamma(u, classes[e]);
        if (sum >= max){
            max = sum;
            argmax = e;

        }
    }
    result.type = argmax;
    if (result.type == 1) result.s0 = 1; //TODO: find intermediate values
    if (result.type == 2) result.s1 = 1;
    if (result.type == 3) result.s2 = 1;
    if (result.type == 4) result.s3 = 1;

    return result;
}

float Q(){ //empirical risk
    float q = 0;
    Y res;
    for (int i = 0; i < counter_train; i++){
        res = algorithm(training_set[i].training_in);
        q += sqrt((res.s0 - training_set[i].training_out.s0)*(res.s0 - training_set[i].training_out.s0) +
                (res.s1 - training_set[i].training_out.s1)*(res.s1 - training_set[i].training_out.s1) +
                (res.s2 - training_set[i].training_out.s2)*(res.s2 - training_set[i].training_out.s2) +
                (res.s3 - training_set[i].training_out.s3)*(res.s3 - training_set[i].training_out.s3));
    }
    return q/(float)counter_train;
}

void init_gamma(){
    gam = new float[counter_train];
    for (int i = 0; i < counter_train; i++){
        gam[i] = 0;

    }

   float q = 1;
   while (q > EPSILON){
       q = Q();
       for (int i = 0; i < counter_train; i++){
            if (algorithm(training_set[i].training_in).type != training_set[i].training_out.type)
                gam[i] += 1;
       }
   }

}


