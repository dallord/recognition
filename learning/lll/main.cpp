#include <QCoreApplication>
#include <math.h>

#include <fstream>
#include <iostream>

#define N 6 //number of components of X vector
#define M 4 //number of components of Y vector

#define K 5 //number of classes

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
    float s3;

    int type;
};

struct Pair{
    X training_in;
    Y training_out;
};

int counter_train; //number of pairs in the training set - l
int counter_learn;

//training pairs
//X* training_in; //list of training vectors - x_i
//Y* training_out; //list of training answers - y_i
Pair* training_set; //list of training pairs - x_i; y_i

X* learning_in; //learning set
Y* check_out; //checking of answers
Y classes[K];

float** r; //array of distances
float* gam; //array of gamma coefficients
float h; //width of the window

void init_classes(){ //init array of existing classes

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

float rho(X u, X x_i){ //distance
    return sqrt(abs(u.x0 - x_i.x0)*abs(u.x0 - x_i.x0) + abs(u.y0 - x_i.y0)*abs(u.y0 - x_i.y0)
                + abs(u.x1 - x_i.x1)*abs(u.x1 - x_i.x1) + abs(u.y1 - x_i.y1)*abs(u.y1 - x_i.y1)
                + abs(u.x2 - x_i.x2)*abs(u.x2 - x_i.x2) + abs(u.y2 - x_i.y2)*abs(u.y2 - x_i.y2));
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

float Gamma(X u, Y y){
    float sum = 0;
    float c = 0;
    for (int i = 0; i < counter_train; i++){
        if (training_set[i].training_out.type == y.type){
            sum += gam[i]*(h/(rho(u, training_set[i].training_in) + 1.));
       // cout << h << endl;
        }
    }
   //cout <<  sum << endl;
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
        /*if (e == 1) result.s0 = sum;
        if (e == 2) result.s1 = sum;
        if (e == 3) result.s2 = sum;
        if (e == 4) result.s3 = sum;*/
        //cout << sum << endl;
        if (sum >= max){
            max = sum;
            argmax = e;

        }
    }
    result.type = argmax;
    if (result.type == 1) result.s0 = 1;
    if (result.type == 2) result.s1 = 1;
    if (result.type == 3) result.s2 = 1;
    if (result.type == 4) result.s3 = 1;
    //cout << argmax << endl;

    return result;
}

float Q(){ //empirical risk
    float q = 0;
    Y res;
    for (int i = 0; i < counter_train; i++){
        res = algorithm(training_set[i].training_in);
       // cout << res.s0 << " " << res.s1 << " " << res.s2 << " " << res.s3 << endl;
        q += sqrt((res.s0 - training_set[i].training_out.s0)*(res.s0 - training_set[i].training_out.s0) +
                (res.s1 - training_set[i].training_out.s1)*(res.s1 - training_set[i].training_out.s1) +
                (res.s2 - training_set[i].training_out.s2)*(res.s2 - training_set[i].training_out.s2) +
                (res.s3 - training_set[i].training_out.s3)*(res.s3 - training_set[i].training_out.s3));
    }
    return q/(float)counter_train;
}

float margin(Pair x_i){
    float G1, G2;
    float max = 0;
    G1 = Gamma(x_i.training_in, x_i.training_out);
    for (int i = 0; i < K; i++){
        if (i != x_i.training_out.type){
            G2 = Gamma(x_i.training_in, classes[i]);
            if (G2 > max) max = G2;
        }

    }
    return G1-G2;
}

void init_gamma(){
    gam = new float[counter_train];
    for (int i = 0; i < counter_train; i++){
        gam[i] = 0;

    }

    float sum_m = 0;
    float av_m = 1;
    float q = 1;
   while (q > 0.01){
       q = Q();
       for (int i = 0; i < counter_train; i++){
            if (algorithm(training_set[i].training_in).type != training_set[i].training_out.type)
                gam[i] += 1;
            //sum_m += margin(training_set[i]);
           // cout << gam[i] << " ";
       }
       //av_m = sum_m/(float)counter_train;
      // cout << endl;
   // cout << av_m << endl;
    //cout << q << endl;
   }
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ifstream infilelearn("../../data/experiments/learning_set.txt"); //open list of reference points for learning
    ifstream infiletrain("../../data/experiments/training_set.txt"); //open file with training set

    ofstream outfile("../../data/experiments/result.txt"); //output file


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
    training_set = new Pair[counter_train];

    learning_in = new X[counter_learn];
    check_out = new Y[counter_learn];
    init_classes();

    //filling training and learning sets
    infiletrain.open("../../data/experiments/training_set.txt");
    string line;
    int i = 0;
    while(!infiletrain.eof()){
        getline(infiletrain, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &training_set[i].training_in.x0, &training_set[i].training_in.y0, &training_set[i].training_in.x1, &training_set[i].training_in.y1, &training_set[i].training_in.x2, &training_set[i].training_in.y2,
               &training_set[i].training_out.s0, &training_set[i].training_out.s1, &training_set[i].training_out.s2, &training_set[i].training_out.s3);
        training_set[i].training_out.type = set_type(training_set[i].training_out);
        i++;
    }
    infiletrain.close();

    h = 3.;
    init_gamma();


    infilelearn.open("../../data/experiments/learning_set.txt");
    i = 0;
    while(!infilelearn.eof()){
        getline(infilelearn, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &learning_in[i].x0, &learning_in[i].y0, &learning_in[i].x1, &learning_in[i].y1, &learning_in[i].x2, &learning_in[i].y2,
                &check_out[i].s0, &check_out[i].s1, &check_out[i].s2, &check_out[i].s3);
        check_out[i].type = set_type(check_out[i]);
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
        A[i] = algorithm(learning_in[i]).type;
        if (A[i] == 0) classname = "no object";
        if (A[i] == 1) classname = "standing";
        if (A[i] == 2) classname = "sitting";
        if (A[i] == 3) classname = "lying on the bed";
        if (A[i] == 4) classname = "lying on the floor";
        outfile << learning_in[i].x0 << " " << learning_in[i].y0 << " " << learning_in[i].x1 << " " << learning_in[i].y1 << " " << learning_in[i].x2 << " " << learning_in[i].y2 << " = " << classname;
        if (A[i] != check_out[i].type) outfile << " - not correct" << endl;
        else outfile << endl;
    }
    outfile.close();
    cout << "Done" << endl;


    return a.exec();
}
