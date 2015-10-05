#ifndef LEARNING_H
#define LEARNING_H

#include <math.h>
#include <vector>

#include <fstream>
#include <iostream>

#define N 6 //number of components of X vector
#define M 3 //number of components of Y vector

#define K 5 //number of classes

#define ENV_MIN 7900
#define ENV_MAX 8000
#define BODY_MIN 8100
#define BODY_MAX 8300

#define EPSILON 0.01

using namespace std;

struct Point{ //object points
    int x;
    int y;
    int weight;
};

struct X{ //input vector of reference point coordinate
    int x0;
    int y0;
    int x1;
    int y1;
    int x2;
    int y2;
};

struct Y{ //otput vector of states
    float s0;
    float s1;
    float s2;
    float s3;

    int type;
};

struct Pair{
    X training_in;
    Y training_out;
};

extern int counter_train; //number of pairs in the training set - l
extern int counter_learn;

//training pairs
//extern X* training_in; //list of training vectors - x_i
//extern Y* training_out; //list of training answers - y_i
extern Pair* training_set; //list of training pairs - x_i; y_i

extern X learning_in; //learning point
extern Y classes[K];

extern float* gam; //array of gamma coefficients
extern float h; //width of the window

X find_ref_points(vector<int> line);

void init_classes(); //init array of existing classes
void init_gamma(); //init gamma coefficients

float Q(); //error

int set_type(Y y); //set id for class

float rho(X u, X x_i); //calculate the distance

Y algorithm(X u);

void learning_alg(X x, Y y, int e);


#endif // LEARNING_H
