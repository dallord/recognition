#ifndef LEARNING_H
#define LEARNING_H

#include <math.h>
#include <vector>

#include <fstream>
#include <iostream>

#define N 6 //number of components of X vector
#define M 3 //number of components of Y vector

#define K 4 //number of classes

using namespace std;

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

    int type;
};

extern int counter_train; //number of pairs in the training set - l
extern int counter_learn;

//training pairs
extern X* training_in; //list of training vectors - x_i
extern Y* training_out; //list of training answers - y_i

extern X learning_in; //learning point
extern Y classes[K];

extern float* gam; //array of gamma coefficients
extern float h; //width of the window

X find_ref_points(vector<int> line);

void init_classes(); //init array of existing classes
void init_gamma(); //init gamma coefficients

void set_type(Y y); //set pointer to class

float rho(X u, X x_i); //calculate distance

float algorithm(X u);


#endif // LEARNING_H
