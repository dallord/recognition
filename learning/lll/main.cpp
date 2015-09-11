#include <QCoreApplication>
#include <math.h>

#include <fstream>
#include <iostream>

#define N 2 //number of components of X vector
#define M 3 //number of components of Y vector

#define K 4 //number of classes

using namespace std;

struct X{ //input vector of reference point coordinate
    int x;
    int y;
};

struct Y{ //otput vector of states
    float s0;
    float s1;
    float s2;

    int type;
};

int counter_train; //number of pairs in the training set - l
int counter_learn;

//training pairs
X* training_in; //list of training vectors - x_i
Y* training_out; //list of training answers - y_i

X* learning_in; //learning set
Y classes[K];

float** r; //array of distances
float gam[10]; //array of gamma coefficients
float h; //width of the window

void init_classes(){ //init array of existing classes

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

float rho(X u, X x_i){ //distance
    return sqrt(abs(u.x - x_i.x)*abs(u.x - x_i.x) + abs(u.y - x_i.y)*abs(u.y - x_i.y));
}

void qsort(float rho[], int left, int right){
    int l = left;
    int r = right;
    float foo = 0;
    float mid = rho[(l+r)/2];
    while (l <= r){
        while ((rho[l] < mid) && (l <= right)) l++;
        while ((rho[r] > mid) && (r >= left)) r--;
        if (l <= r){
            foo = rho[l];
            rho[l] = rho[r];
            rho[r] = foo;
            l++;
            r--;
        }

    }
    if (r > left) qsort(rho, left, r);
    if (l < right) qsort(rho, l, right);
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


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ifstream infilelearn("../../data/ref_points.txt"); //open list of reference points for learning
    ifstream infiletrain("../../data/training_set.txt"); //open file with training set

    ofstream outfile("../../data/result.txt"); //output file


    //count number of training and learning vectors
    string buff;
    counter_train = 0;
    while (!infiletrain.eof()){
        getline(infiletrain, buff);
        counter_train++;
    }
    infiletrain.close();
    counter_learn = 0;
    while (!infilelearn.eof()){
        getline(infilelearn, buff);
        counter_learn++;
    }
    infilelearn.close();

    counter_train--;
    counter_learn--;

    //init sets and coefficients
    training_in = new X[counter_train];
    training_out = new Y[counter_train];

    //gam = new float[counter_train];
    init_gamma();
    h = 5;

    learning_in = new X[counter_learn];
    init_classes();

    //filling training and learning sets
    infiletrain.open("../../data/training_set.txt");
    string line;
    int i = 0;
    while(!infiletrain.eof()){
        getline(infiletrain, line);
        sscanf(line.c_str(), "%d %d %f %f %f",
               &training_in[i].x, &training_in[i].y,
               &training_out[i].s0, &training_out[i].s1, &training_out[i].s2);
        set_type(training_out[i]);
        i++;
    }
    infiletrain.close();


    infilelearn.open("../../data/ref_points.txt");
    i = 0;
    while(!infilelearn.eof()){
        getline(infilelearn, line);
        sscanf(line.c_str(), "%d %d",
               &learning_in[i].x, &learning_in[i].y);
        i++;
    }
    infilelearn.close();

    //for (int i = 0 ; i < counter_train-1; i++)
    //    cout << training_in[i].x << " " << training_in[i].y << "| " << training_out[i].s0 << " " << training_out[i].s1 << " " << training_out[i].s2 << endl;
    //for (int i = 0 ; i < counter_learn-1; i++)
    //    cout << learning_in[i].x << " " << learning_in[i].y <<  endl;

    /*float** r; //array of distances
    r = new float*[counter_train];
    for (int e = 0; e < counter_learn; e++)
        r[e] = new float[counter_learn];

    for (int i = 0; i < counter_learn; i++){
        for (int j = 0; j < counter_train; j++){
            r[i][j] = rho(learning_in[i], training_in[j]);
        }
    }

    for (int i = 0; i < counter_learn; i++){
        qsort(r[i], 0, counter_train);
        for (int j = 0; j < counter_train; j++){
            cout << r[i][j] << " ";
        }
    cout << endl;
    }*/

    string classname;
    int A[counter_learn];
    for (int i = 0; i < counter_learn; i++){
        A[i] = algorithm(learning_in[i]);
        if (A[i] == 0) classname = "no object";
        if (A[i] == 1) classname = "standing";
        if (A[i] == 2) classname = "sitting";
        if (A[i] == 3) classname = "lying";
        outfile << learning_in[i].x << " " << learning_in[i].y << " = " << classname << endl;
    }
    outfile.close();
    cout << "Done" << endl;


    return a.exec();
}
