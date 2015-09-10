#include <QCoreApplication>
#include <math.h>

#include <fstream>
#include <iostream>

#define N 2 //number of components of X vector
#define M 3 //number of components of Y vector

using namespace std;

struct X{ //input vector of reference point coordinate
    int x;
    int y;
};

struct Y{ //otput vector of states
    float s0;
    float s1;
    float s2;
};



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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ifstream infilelearn("../../data/ref_points.txt"); //open list of reference points for learning
    ifstream infiletrain("../../data/training_set.txt"); //open file with training set

    ofstream outfile("../../data/result.txt"); //output file
    
    X* training_in; //list of training vectors
    Y* training_out; //list of training answers
    X* learning_in; //learning set

    //count number of training and learning vectors
    string buff;
    int counter_train = 0;
    while (!infiletrain.eof()){
        getline(infiletrain, buff);
        counter_train++;
    }
    infiletrain.close();
    int counter_learn = 0;
    while (!infilelearn.eof()){
        getline(infilelearn, buff);
        counter_learn++;
    }
    infilelearn.close();

    counter_train--;
    counter_learn--;

    training_in = new X[counter_train];
    training_out = new Y[counter_train];

    learning_in = new X[counter_learn];

    //filling training and learning sets
    infiletrain.open("../../data/training_set.txt");
    string line;
    int i = 0;
    while(!infiletrain.eof()){
        getline(infiletrain, line);
        sscanf(line.c_str(), "%d %d %f %f %f",
               &training_in[i].x, &training_in[i].y,
               &training_out[i].s0, &training_out[i].s1, &training_out[i].s2);
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

    float** r; //array of distances
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
    }

    return a.exec();
}
