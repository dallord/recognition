#include "learning.h"

int counter_train = 0; //number of pairs in the training set - l

//training pairs
X* training_in; //list of training vectors - x_i
Y* training_out; //list of training answers - y_i

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

float Gamma(X u, Y y){
    int b; //true or false
    float sum = 0;
    for (int i = 0; i < counter_train; i++){
        if (    (training_out[i].s0 == y.s0) &&
                (training_out[i].s1 == y.s1) &&
                (training_out[i].s2 == y.s2))
            b = 1;
        else b = 0;
        sum += b*gam[i]*(h/(rho(u, training_in[i]) + 1));
    }
    return sum;
}

Y algorithm(X u){
    int argmax = 0; //will return argument of maximum value
    float max = 0; //maximum value
    float sum = 0;

    for (int e = 0; e < K; e++){
        sum = Gamma(u, classes[e]);
        //cout << sum << endl;
        if (e == 1) learning_out.s0 = sum;
        if (e == 2) learning_out.s1 = sum;
        if (e == 3) learning_out.s2 = sum;
        if (sum > max){
            max = sum;
            argmax = e;
        }
    }
    learning_out.type = argmax;

    return learning_out;
}

float Q(){ //empirical risk
    float q = 0;
    for (int i = 0; i < counter_train; i++){
        q += sqrt((algorithm(training_in[i]).s0 - training_out[i].s0)*(algorithm(training_in[i]).s0 - training_out[i].s0) +
                (algorithm(training_in[i]).s1 - training_out[i].s1)*(algorithm(training_in[i]).s1 - training_out[i].s1) +
                (algorithm(training_in[i]).s2 - training_out[i].s2)*(algorithm(training_in[i]).s2 - training_out[i].s2));
    }
    return q/(float)counter_train;
}

void init_gamma(){
    for (int i = 0; i < counter_train; i++){
        gam[i] = 0;
    }
    for (int i = 0; i < counter_train; i++){
        if (algorithm(training_in[i]).type != training_out[i].type)
            gam[i]++;
    }

}

void qsort(float rho[], X training[], Y out[], int left, int right){
    int l = left;
    int r = right;
    float foo = 0;
    X temp;
    Y temp1;
    float mid = rho[(l+r)/2];
    while (l <= r){
        while ((rho[l] < mid) && (l <= right)) l++;
        while ((rho[r] > mid) && (r >= left)) r--;
        if (l <= r){
            foo = rho[l];

            temp.x0 = training[l].x0; temp.y0 = training[l].y0;
            temp.x1 = training[l].x1; temp.y1 = training[l].y1;
            temp.x2 = training[l].x2; temp.y2 = training[l].y2;

            temp1.s0 = out[l].s0; temp1.s1 = out[l].s1; temp1.s2 = out[l].s2;

            rho[l] = rho[r];

            training[l].x0 = training[r].x0; training[l].y0 = training[r].y0;
            training[l].x1 = training[r].x1; training[l].y1 = training[r].y1;
            training[l].x2 = training[r].x2; training[l].y2 = training[r].y2;

            out[l].s0 = out[r].s0; out[l].s1 = out[r].s1; out[l].s2 = out[r].s2;

            rho[r] = foo;

            training[r].x0 = temp.x0; training[r].y0 = temp.y0;
            training[r].x1 = temp.x1; training[r].y1 = temp.y1;
            training[r].x2 = temp.x2; training[r].y2 = temp.y2;

            out[r].s0 = temp1.s0; out[r].s1 = temp1.s1; out[r].s2 = temp1.s2;

            l++;
            r--;
        }

    }
    if (r > left) qsort(rho, training, out, left, r);
    if (l < right) qsort(rho, training, out, l, right);
}

void learning_alg(X x, Y y, int e){
    init_gamma();
    float r[counter_train];
    for (int i = 0; i < counter_train; i++){
        r[i] = rho(x, training_in[i]);
        cout << r[i] << endl;
    }
    qsort(r, training_in, training_out, 0, counter_train);
    for (int i = 0; i < counter_train; i++)
        cout << r[i] << endl;
   /* while (Q() > EPSILON){
        if (algorithm(x).type != y.type)
            gam[e] += 1;
        cout << Q() << endl;
    }*/
}
