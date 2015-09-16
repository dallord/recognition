#include <QCoreApplication>

#include <fstream>
#include <iostream>

#include <vector>
#include <math.h>

using namespace std;

int w = 80;
int h = 60;

struct X{
    int x;
    int y;
};

struct Point{
    X coord;
    int weight;
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

    ifstream infile("../../data/outData.txt"); //open data-file

    ofstream outfile("../../data/ref_points.txt"); //output file
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
    infile.open("../../data/outData.txt");
    while (!infile.eof()){
        getline(infile, buff);
        counter++;
    }
    infile.close();


    vector<int> data[counter]; //data array
    vector<Point> object; //points of the object
    Point p;
    X c;//center of the mass

    for (int j = 0; j < counter; j++){
        for(int i = 0; i < vct.size()/counter; i++){
            data[j].push_back(vct[j*vct.size()/counter+i]);
        }
    }

    //find coordinates of maximum value in each line
    int max;
    X m; //head
    X l; //legs
    int d;
    int m_dist; //maximum distance
    for (int j = 0; j < counter; j++){
        max = 8000; //detect object
        m_dist = 0;
        //second point
        for (int i = 0; i < vct.size()/counter; i++){
            if (data[j][i] > max){
                p.weight = data[j][i];
                p.coord.x = i%w;
                p.coord.y = i/w;
                object.push_back(p);
                outobj << p.coord.x << " " << p.coord.y << ": " << p.weight << " ";
            }

        }
        c = find_center_mass(object);
        outobj << endl;

        //first point
        for (int i = 0; i < vct.size()/counter; i++){
            if (data[j][i] > max){
                max = data[j][i];
                m.x = i%w;
                m.y = i/w;
            }
        }
        if (max != 8000){
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
            outfile << m.x << " " << m.y << " " << c.x << " " << c.y << " " << l.x << " " << l.y<< "\n";

        }
        else {
            cout << "no object" << endl;
            outfile << "-1 -1 -1 -1 -1 -1\n";
        }

        object.clear();
    }
    outfile.close();
    outobj.close();


    return a.exec();
}
