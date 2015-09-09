#include <QCoreApplication>

#include <fstream>
#include <iostream>

#include <vector>

using namespace std;

int w = 80;
int h = 60;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    string buff; //line buffer
    int ss; //numbers
    char temp[4];

    long counter = 0;

    vector<int> vct;

    ifstream file("../../data/outData.txt"); //open data-file

    if (file.is_open()){

        while(!file.eof()){
            file >> ss;
            vct.push_back(ss);
        }


        //cout << vct.size() << endl;
    }else{
        cout << "Unable to open file" << endl;
    }
    file.close();

    file.open("../../data/outData.txt");
    while (!file.eof()){
        getline(file, buff);
        counter++;
    }
    file.close();


    vector<int> data[counter]; //data array

    for (int j = 0; j < counter; j++){
        for(int i = 0; i < vct.size()/counter; i++){
            data[j].push_back(vct[j*vct.size()/counter+i]);
        }
    }

    //find coordinates of maximum value in each line
    int max;
    int x, y;
    for (int j = 0; j < counter; j++){
        max = 8000; //detect object
        x = 0;
        y = 0;
        for (int i = 0; i < vct.size()/counter; i++){
            if (data[j][i] > max){
                max = data[j][i];
                x = i%w;
                y = i/w;
            }
        }
        if (max != 8000)
            cout << max << ": " << x << " " << y << endl;
        else cout << "no object" << endl;
    }



    return a.exec();
}
