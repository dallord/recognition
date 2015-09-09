#include <QCoreApplication>

#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    string buff; //line buffer
    int *ss; //numbers

    ifstream file("../data/outData.txt"); //open data-file

    if (file.is_open()){
        file.seekg (0, ios::end);
        long fileSize = file.tellg();
        file.seekg(0, ios::beg);

        ss = (int*) malloc (fileSize*sizeof(int));

        file.getline(buff, 24000); //reading first line

        for (int i = 0; i <= buff.Length(); i++){
            if (buff[i] != " "){

            }
        }

        file.close();
    }


    return a.exec();
}
