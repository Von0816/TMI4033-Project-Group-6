/*
Original Author: Guang Yang
*/

// modified by Boonlert Randy (74297) for Group 6 TMI4033 Project

//This file is used to generate predictions.
#include <cmath>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <string>
#include <string.h>
#include <utility>
#include "SVD.cpp"

using namespace std;

int main() {
    string trainFile = "train.txt";  //set train data
    string crossFile = "cross.txt";  //set cross validation data
    string testFile = "test_data.txt";  //set test data
    string outFile = "output.txt";  //set output data
    ifstream fp("training_data.txt");
    ofstream ft("train.txt");
    ofstream fc("cross.txt");
    srand(time(NULL));
    char s[2048];
    cout<<"scanning training file"<<endl;
    while (fp.getline(s, 2000)) {
        if (rand()%10==0) {
            fc << s << endl;
        }
        else {
            ft << s << endl;
        }
    }
    cout<<"added to force make lmao"<<endl;
    cout<<"training data scanned"<<endl;
    fp.close();
    ft.close();
    fc.close();
    cout<<"i/o stream closed"<<endl;
    cout<<"instantiating svd module"<<endl;
    SVD svd(NULL,NULL,0,NULL,NULL, trainFile, crossFile, testFile, outFile);
    double rmse = svd.MyTrain();     //train
    return 0;
}
