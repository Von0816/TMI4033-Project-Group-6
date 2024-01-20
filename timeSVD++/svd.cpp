/*
Original Author: Guang Yang
*/

// modified by Boonlert Randy (74297) for Group 6 TMI4033 Project

//This file is the implementation of the class SVD.
#include <cmath>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include <ctime>
#include <utility>
#include "SVD.h"
#define sign(n) (n==0? 0 : (n<0?-1:1))    //define sign function

using namespace std;

const long int userNum = 2649430;  //number of users
const int itemNum = 11;   //number of items
const long int timeNum = 13150;    //number of days(time)
const int binNum = 30;       //number of time bins
double AVG = 0;  //average score
double G_alpha = 0.00001;        //gamma for alpha
const double L_alpha = 0.0004;   //learning rate for alpha
const double L_pq = 0.001;       //learning rate for Pu & Qi
double G = 0.007;                //general gamma
const double Decay = 0.95;        //learning rate decay factor
const double L = 0.005;          //general learning rate
const int factor = 50;           //number of factors

//calculate dev_u(t) = sign(t-tu)*|t-tu|^0.4 and save the result for saving the time
double SVD::CalDev(int user, int timeArg) {
    if(Dev[user].count(timeArg)!=0)return Dev[user][timeArg];
    double tmp = sign(timeArg - Tu[user]) * pow(double(abs(timeArg - Tu[user])), 0.4);
    Dev[user][timeArg] = tmp;
    return tmp;
}

//calculate time bins
int SVD::CalBin(int timeArg) {
    int binsize = timeNum/binNum + 1;
    return timeArg/binsize;
}

//main function for training
//terminate when RMSE varies less than 0.00005
double SVD::MyTrain() {
  double preRmse = 1000;
  ofstream fout(outFile.c_str());
  ofstream sBi("../saved_params/Bi.txt");
  ofstream sBu("../saved_params/Bu.txt");
  ofstream sAlpha_U("../saved_params/Alpha_u.txt");
  ofstream sTu("../saved_params/Tu.txt");
  ofstream sBi_Bin("../saved_params/Bi_Bin.txt");
  ofstream sBu_t("../saved_params/Bu_t.txt");
  ofstream sDev("../saved_params/Dev.txt");
  ofstream sQi("../saved_params/Qi.txt");
  ofstream sPu("../saved_params/Pu.txt");
  ofstream ssumMW("../saved_params/sumMW.txt");
  ofstream sSz("../saved_params/Sz.txt");
  srand(time(NULL));
  FILE *fp = fopen(testFile.c_str(),"r");
  //FILE *fq = fopen(trainFile.c_str(), "r");
  int user, item, date, rating;
  double curRmse;
  for(size_t i=0;i<1000;i++) {
    Train();
    curRmse = Validate(AVG,Bu,Bi,Pu,Qi);
    cout << "test_Rmse in step " << i << ": " << curRmse << endl;
    if(curRmse >= preRmse-0.00001){
      cout<<"min rmse reached"<<endl;
      cout<<"normalized rmse: "<<curRmse/(5-1)<<endl;
      break;
    }
    else{
      preRmse = curRmse;
    }
  }
  //avg + Bu[userId] + Bi[itemId] + Bi_Bin[itemId][CalBin(time)] + Alpha_u[userId]*CalDev(userId,time) + Bu_t[userId][time] + tmp;
  while (fscanf(fp,"%d,%d,%d",&user, &item, &date)!=EOF) {
  //     fout <<user<<","<<item<<","<<predictScore(AVG,user,item,date)<<","<<Bi[item] << endl;
    fout<<"User Id: "<<user<<endl;
    fout<<"item Id: "<<item<<endl;
    fout<<"day after epoch: "<<date<<endl;
    fout<<"average rating: "<<AVG<<endl;
    fout<<"Bu: "<<Bu[user]<<endl;
    fout<<"Bi: "<<Bi[item]<<endl;
    fout<<"Bi_Bin: "<<Bi_Bin[item][CalBin(date)]<<endl;
    fout<<"CalBin: "<<CalBin(date)<<endl;
    fout<<"Alpha_u: "<<Alpha_u[user]*CalDev(user, date)<<endl;
    fout<<"Bu_t: "<<Bu_t[user][date]<<endl;
    fout<<"predicted score: "<<predictScore(AVG, user, item, date, false)<<endl;
  }

  cout<<"Saving Bi"<<endl;
  //save Bi
  for(int i = 0; i < itemNum; i++){
    if(Bi[i] != 0) {
      sBi<<i<<","<<Bi[i]<<endl;
    }
  }
  cout<<"Saving Bu"<<endl;
  //save Bu
  for(int i = 0; i < userNum; i++){
    if(Bu[i] != 0){
      sBu<<i<<","<<Bu[i]<<endl;
    }
  }
  cout<<"Saving Alpha_u"<<endl;
  //save Alpha_u
  for(int i = 0; i < userNum; i++){
    if (Alpha_u[i] != 0) {
      sAlpha_U<<i<<","<<Alpha_u[i]<<endl;
    }
  }
  cout<<"Saving Train size"<<endl;
  //save Dev size
  for(int i = 0; i < userNum; i++){
    if (train_data[i].size() != 0) {
      sSz<<i<<","<<train_data[i].size()<<endl;
    }
  }
  cout<<"Saving Tu"<<endl;
  //save Tu
  for(int i = 0; i < userNum; i++){
    if (Tu[i] != 0) {
      sTu<<i<<","<<Tu[i]<<endl;
    }
  }
  cout<<"Saving Bi_Bin"<<endl;
  //save Bi_Bin
  for(int i = 0; i < itemNum; i++){
    for(int j = 0; j < binNum; j++){
      if (Bi_Bin[i][j] !=0) {
        sBi_Bin<<i<<","<<j<<","<<Bi_Bin[i][j]<<endl;
      }
    }
  }
  cout<<"Saving Pu"<<endl;
  //save Pu
  for(int i = 0; i < userNum; i++){
    for(int j = 0; j < factor; j++){
      if (Pu[i][j] !=0) {
        sPu<<i<<","<<j<<","<<Pu[i][j]<<endl;
      }
    }
  }
  cout<<"Saving Qi"<<endl;
  //save Qi
  for(int i = 0; i < itemNum; i++){
    for(int j = 0; j < factor; j++){
      if (Qi[i][j] !=0) {
        sQi<<i<<","<<j<<","<<Qi[i][j]<<endl;
      }
    }
  }
  cout<<"Saving sumMW"<<endl;
  //save sumMW
  for(int i = 0; i < userNum; i++){
    for(int j = 0; j < factor; j++){
      if (sumMW[i][j] !=0) {
        ssumMW<<i<<","<<j<<","<<sumMW[i][j]<<endl;
      }
    }
  }
  cout<<"Saving Bu_t"<<endl;
  //save Bu_t
  for(int i = 0; i < userNum; i++){
    for(int j = 0; j < userDateList[i].size(); j++){
      if (Bu_t[i][userDateList[i][j]] !=0) {
        sBu_t<<i<<","<<userDateList[i][j]<<","<<Bu_t[i][userDateList[i][j]]<<endl;
      }
    }
  }
  cout<<"Saving Dev"<<endl;
  //save Dev
  for(int i = 0; i < userList.size(); i++){
    for(int j = 0; j < userDateList[userList[i]][j]; j++){
      if (Dev[userList[i]][userDateList[userList[i]][j]] !=0) {
        sDev<<userList[i]<<","<<userDateList[userList[i]][j]<<","<<Dev[userList[i]][userDateList[userList[i]][j]]<<endl;
      }
    }
  }

  fclose(fp);
  fout.close();
  sBi.close();
  sBu.close();
  sAlpha_U.close();
  sTu.close();
  sBi_Bin.close();
  sBu_t.close();
  sDev.close();
  sQi.close();
  sPu.close();
  ssumMW.close();
  sSz.close();
  return curRmse;
}


//function for cross validation
double SVD::Validate(double avg,double* bu,double* bi,double** pu,double** qi){
    int userId,itemId,rating,t;
    int n = 0;
    double rmse = 0;
    for (const auto &ch:test_data){
        userId = ch.first.first;
        itemId = ch.first.second;
        t = ch.second.first;
        rating = ch.second.second;
        n++;
        double pScore = predictScore(avg,userId,itemId,t, true);
        rmse += (rating - pScore) * (rating - pScore);
    }
    return sqrt(rmse/n);
}


//function for prediction
//   prediction formula:
//   avg + Bu + Bi
//   + Bi_Bin,t + Alpha_u*Dev + Bu_t
//   + Qi^T(Pu + |R(u)|^-1/2 \sum yi
double SVD::predictScore(double avg,int userId, int itemId,int time, bool isTraining){
    double tmp = 0.0;
    int sz = train_data[userId].size();
    double sqrtNum = 0;
    if (sz>1) sqrtNum = 1/(sqrt(sz));
    if (!isTraining) cout<<"sqrtNum: "<<sqrtNum<<endl;
    for(size_t i=0;i<factor;i++){
        tmp += (Pu[userId][i] + sumMW[userId][i]*sqrtNum) * Qi[itemId][i];
        if(!isTraining){
          cout<<"current factor: "<<i<<endl;
          cout<<"tmp: "<<tmp<<endl;
          cout<<"Pu: "<<Pu[userId][i]<<endl;
          cout<<"sumMW: "<<sumMW[userId][i]<<endl;
          cout<<"Qi: "<<Qi[itemId][i]<<endl;
        }
    }
    double score = avg + Bu[userId] + Bi[itemId] + Bi_Bin[itemId][CalBin(time)] + Alpha_u[userId]*CalDev(userId,time) + Bu_t[userId][time] + tmp;

    if(score > 5){
        score = 5;
    }
    if(score < 1){
        score = 1;
    }
    return score;
}

//function for training
//update using stochastic gradient descent

void SVD::Train(){
    int userId,itemId,rating,time;
    for (userId = 0; userId < userNum; ++userId) {
        int sz = train_data[userId].size();
        double sqrtNum = 0;
        vector <double> tmpSum(factor,0);
        if (sz>1) sqrtNum = 1/(sqrt(sz));
        for (int k = 0; k < factor; ++k) {
            double sumy = 0;
            for (int i = 0; i < sz; ++i) {
                int itemI = train_data[userId][i].first.first;
                sumy += y[itemI][k];
            }
            sumMW[userId][k] = sumy;
        }
        for (int i = 0; i < sz; ++i) {
            itemId = train_data[userId][i].first.first;
            rating = train_data[userId][i].first.second;
            time = train_data[userId][i].second;
            double predict = predictScore(AVG,userId,itemId,time, true);
            double error = rating - predict;
            Bu[userId] += G * (error - L * Bu[userId]);
            Bi[itemId] += G * (error - L * Bi[itemId]);
            Bi_Bin[itemId][CalBin(time)] += G * (error - L * Bi_Bin[itemId][CalBin(time)]);
            Alpha_u[userId] += G_alpha * (error * CalDev(userId,time)  - L_alpha * Alpha_u[userId]);
            Bu_t[userId][time] += G * (error - L * Bu_t[userId][time]);

            for(size_t k=0;k<factor;k++){
                auto uf = Pu[userId][k];
                auto mf = Qi[itemId][k];
                Pu[userId][k] += G * (error * mf - L_pq * uf);
                Qi[itemId][k] += G * (error * (uf+sqrtNum*sumMW[userId][k]) - L_pq * mf);
                tmpSum[k] += error*sqrtNum*mf;
            }
        }
        for (int j = 0; j < sz; ++j) {
            itemId = train_data[userId][j].first.first;
            for (int k = 0; k < factor; ++k) {
                double tmpMW = y[itemId][k];
                y[itemId][k] += G*(tmpSum[k]- L_pq *tmpMW);
                sumMW[userId][k] += y[itemId][k] - tmpMW;
            }
        }
    }
    for (userId = 0; userId < userNum; ++userId) {
        auto sz = train_data[userId].size();
        double sqrtNum = 0;
        if (sz>1) sqrtNum = 1.0/sqrt(sz);
        for (int k = 0; k < factor; ++k) {
            double sumy = 0;
            for (int i = 0; i < sz; ++i) {
                int itemI = train_data[userId][i].first.first;
                sumy += y[itemI][k];
            }
            sumMW[userId][k] = sumy;
        }
    }
    G *= Decay;
    G_alpha *= Decay;
}

//initialization

SVD::SVD(double* bi,double* bu,int k,double** qi,double** pu, string train_file, string cross_file, string test_file, string out_file):
 trainFile(train_file), crossFile(cross_file), testFile(test_file), outFile(out_file){
    train_data.resize(userNum);
    if(bi == NULL){
        Bi = new double[itemNum];
        for(size_t i=0;i<itemNum;i++){
            Bi[i] = 0.0;
        }
    }
    else{
        Bi = bi;
    }

    if(bu == NULL){
        Bu = new double[userNum];
        for(size_t i=0;i<userNum;i++){
            Bu[i] = 0.0;
        }
    }
    else{
        Bu = bu;
    }

    Alpha_u = new double[userNum];
    for(size_t i=0;i<userNum;i++){
        Alpha_u[i] = 0;
    }

    Bi_Bin = new double* [itemNum];
    for(size_t i=0;i<itemNum;i++){
        Bi_Bin[i] = new double[binNum];
    }

    for(size_t i=0;i<itemNum;i++){
        for(size_t j=0;j<binNum;j++){
            Bi_Bin[i][j] = 0.0;
        }
    }


    if(qi == NULL){
        Qi = new double* [itemNum];
        y = new double* [itemNum];
        for(size_t i=0;i<itemNum;i++){
            Qi[i] = new double[factor];
            y[i] = new double[factor];
        }

        for(size_t i=0;i<itemNum;i++){
            for(size_t j=0;j<factor;j++){
                Qi[i][j] = 0.1 * (rand() / (RAND_MAX + 1.0)) / sqrt(factor);
                y[i][j] = 0;
            }
        }
    }
    else{
        Qi = qi;
    }

    if(pu == NULL){
        sumMW = new double* [userNum];
        Pu = new double* [userNum];
        for(size_t i=0;i<userNum;i++){
            Pu[i] = new double[factor];
            sumMW[i] = new double[factor];
        }

        for(size_t i=0;i<userNum;i++){
            for(size_t j=0;j<factor;j++){
                sumMW[i][j] = 0.1 * (rand() / (RAND_MAX + 1.0)) / sqrt(factor);
                Pu[i][j] = 0.1 * (rand() / (RAND_MAX + 1.0)) / sqrt(factor);
            }
        }
    }   else{
        Pu = pu;
    }
    cout<<"variables initialized"<<endl;
    cout<<"opening train file"<<endl;
    FILE *fp = fopen(trainFile.c_str(),"r");
    int userId,itemId,rating,t, totalRating = 0, ratingCount = 0;
    while(fscanf(fp,"%d,%d,%d,%d", &userId,&itemId,&rating,&t)!=EOF){
        train_data[userId].push_back(make_pair(make_pair(itemId,rating),t));
        userList.push_back(userId);
        userDateList[userId].push_back(t);
        totalRating += rating;
        ratingCount += 1;
    }
    AVG = double(totalRating)/double(ratingCount);
    cout<<"total rating: "<<totalRating<<endl;
    cout<<"rating count: "<<ratingCount<<endl;
    cout<<"average rating: "<<AVG<<endl;
    cout<<"train file scanned"<<endl;
    fclose(fp);
    cout<<"opening cross file"<<endl;
    fp = fopen(crossFile.c_str(),"r");
    while(fscanf(fp,"%d,%d,%d,%d",&userId, &itemId, &rating, &t)!=EOF){
        test_data.push_back(make_pair(make_pair(userId, itemId),make_pair(t,rating)));
    }
    cout<<"cross file scanned"<<endl;
    fclose(fp);

    Tu = new double[userNum];
    for(size_t i=0;i<userNum;i++){
        double tmp = 0;
        if(train_data[i].size()==0)
        {
            Tu[i] = 0;
            continue;
        }
        for(size_t j=0;j<train_data[i].size();j++){
            tmp += train_data[i][j].second;
        }
        Tu[i] = tmp/train_data[i].size();
    }

    for(size_t i=0;i<userNum;i++){
        map<int,double> tmp;
        for(size_t j=0;j<train_data[i].size();j++){
            if(tmp.count(train_data[i][j].second)==0)
            {
                tmp[train_data[i][j].second] = 0.0000001;
            }
            else continue;
        }
        Bu_t.push_back(tmp);
    }

    for(size_t i=0;i<userNum;i++){
        map<int,double> tmp;
        Dev.push_back(tmp);
    }

}

SVD::~SVD(){
    delete[] Bi;
    delete[] Bu;
    delete[] Alpha_u;
    delete[] Tu;
    for(size_t i=0;i<userNum;i++){
        delete[] Pu[i];
        delete[] sumMW[i];
    }
    for(size_t i=0;i<itemNum;i++){
        delete[] Bi_Bin[i];
        delete[] Qi[i];
        delete[] y[i];
    }
    delete[] Bi_Bin;
    delete[] sumMW;
    delete[] y;
    delete[] Pu;
    delete[] Qi;
}
