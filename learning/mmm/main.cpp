#include <QCoreApplication>

#include <fstream>
#include <iostream>

#include <vector>
#include <math.h>

using namespace std;

#define ENV_MIN 7900
#define ENV_MAX 8000
#define BODY_MIN 8100
#define BODY_MAX 8300

int w = 80;
int h = 60;

struct X{
    int x;
    int y;
};

struct Point{
    X coord;
    int weight;
    bool ob; //object or not
};

X find_center_mass(vector<Point> obj){
    X c;
    int t_x = 0;
    int t_y = 0;
    int b = 0;
    for (int i = 0; i < obj.size(); i++){
        t_x += obj[i].weight*obj[i].coord.x;
        t_y += obj[i].weight*obj[i].coord.y;
        b += obj[i].weight;
    }
    if (b != 0){
        c.x = t_x/b;
        c.y = t_y/b;
    } else {
        c.x = 0;
        c.y = 0;
    }
    return c;
}

int dist(X p_0, X p_1){
    int d = sqrt((float)((p_0.x - p_1.x)*(p_0.x - p_1.x) + (p_0.y - p_1.y)*(p_0.y - p_1.y)));
    return d;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    string buff; //line buffer
    int ss; //numbers
    char temp[4];

    long counter = 0;

    vector<int> vct;

    ifstream infile("../../data/experiments/test0.txt"); //open data-file

    //ofstream outfile("../../data/experiments/test5_ref.txt"); //output file
    ofstream outobj("../../data/object.txt");

    if (infile.is_open()){

        while(!infile.eof()){
            infile >> ss;
            vct.push_back(ss);
        }


        //cout << vct.size() << endl;
    }else{
        cout << "Unable to open file" << endl;
    }
    infile.close();

    //count lines (number of seconds) in the file
    infile.open("../../data/experiments/test0.txt");
    while (!infile.eof()){
        getline(infile, buff);
        counter++;
    }
    infile.close();


    vector<Point> data[counter]; //data array
   // vector<Point> object; //points of the object
    Point p;
    X c;//center of the mass

    for (int j = 0; j < counter; j++){
        for(int i = 0; i < vct.size()/counter; i++){
            p.weight = vct[j*vct.size()/counter+i];
            p.coord.x = i%w;
            p.coord.y = i/w;
            p.ob = false;
            data[j].push_back(p);
        }
    }

    //find coordinates of maximum value in each line
    int max;
    X m; //head
    X l; //legs
    int d;
    int m_dist; //maximum distance
    for (int j = 1; j < counter; j++){
        //max = ENV_MAX; //detect object
        //m_dist = 0;
        cout << j << ": ";
        //second point
        for (int i = 0; i < vct.size()/counter; i++){
            if (data[j][i].weight - data[j-1][i].weight > 100){
            //if ((data[j][i] > max) && (data[j][i] < BODY_MAX)){
                cout << data[j][i].weight - data[j-1][i].weight << " ";
                data[j][i].ob = true;
            }
            if (data[j][i].weight - data[j-1][i].weight < -100){
                data[j][i].ob = false;
            }

        }
        for (int e = 0; e < vct.size()/counter; e++){
            if (data[j][e].ob == true)
                outobj << data[j][e].coord.x << " " << data[j][e].coord.y << ": " << data[j][e].weight << " ";
        }
        //c = find_center_mass(object);
        cout << endl;
        outobj << endl;

        //first point
        /*for (int i = 0; i < vct.size()/counter; i++){
            if ((data[j][i] > max) && (data[j][i] < BODY_MAX)){
                max = data[j][i];
                m.x = i%w;
                m.y = i/w;
            }
        }
        if (max != ENV_MAX){
            //third point
            for (int e = 0; e < object.size(); e++) {
                d = dist(m, object[e].coord);
                if (d > m_dist){
                    m_dist = d;
                    l.x = object[e].coord.x;
                    l.y = object[e].coord.y;
                }
            }
            cout << max << ": " << m.x << " " << m.y << "; " << c.x << " " << c.y << "; " << l.x << " " << l.y << endl;
            outfile << "sec " << j+1 << ": " << m.x << " " << m.y << " " << c.x << " " << c.y << " " << l.x << " " << l.y<< "\n";

        }
        else {
            cout << "no object" << endl;
            outfile << "sec " << j+1 << ": " << "-1 -1 -1 -1 -1 -1\n";
        }*/

        //object.clear();
    }
    //outfile.close();
    outobj.close();
    cout << "done" << endl;


    return a.exec();
}
