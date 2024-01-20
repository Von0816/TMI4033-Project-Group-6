#   written by Boonlert Randy (74297) for Group 6 TMI4033 Project

from math import sqrt
from datetime import date

CONST_userCount = 2649430 #number of users in the dataset
CONST_movCount = 11 #number of movies in the dataset
CONST_dayCount = 13150 #number of days after epoch
CONST_binCount = 30 #number of time bins
CONST_avg = 3.27303 #average score in the dataset
CONST_factor = 50 #number of factor

CONST_EPOCH = date(1970, 1, 1) 

CONST_Dir = "saved_params"
FILE_Bi = open(CONST_Dir + "/Bi.txt")
FILE_Bu = open(CONST_Dir + "/Bu.txt") 
FILE_Alpha_U = open(CONST_Dir + "/Alpha_u.txt")
FILE_Tu = open(CONST_Dir + "/Tu.txt")
FILE_Bi_Bin = open(CONST_Dir + "/Bi_Bin.txt")
FILE_Bu_t = open(CONST_Dir + "/Bu_t.txt")
FILE_Dev = open(CONST_Dir + "/Dev.txt")
FILE_Qi = open(CONST_Dir + "/Qi.txt")
FILE_Pu = open(CONST_Dir + "/Pu.txt")
FILE_sumMW = open(CONST_Dir + "/sumMW.txt")
FILE_Sz = open(CONST_Dir + "/Sz.txt")
FILE_test = open("test.txt")

def sign(n):
    if n > 0:
        return 1 
    elif n < 0:
        return -1
    else:
        return 0

def date2day(dateArg):
    year, month, day = dateArg.split('-')
    t_delta = date(int(year), int(month), int(day)) - CONST_EPOCH
    return t_delta.days 

class svd:
    def __init__(self):
        self.Bi = {}
        self.Bu = {}
        self.Alpha_u = {}
        self.Tu = {}
        self.Bi_Bin = {}
        self.Pu = {}
        self.Qi = {}
        self.sumMW = {}
        self.Bu_t = {}
        self.Dev = {}
        self.Sz = {}
        currUserID = 0
        currMovieID = 0
        #load computed Bi
        print("Loading Parameters...")
        for line in FILE_Bi:
            movieID, Bi = line.split(",")
            self.Bi[movieID] = float(Bi[:-1])

        #load computed Bu
        for line in FILE_Bu:
            userID, Bu = line.split(",") 
            self.Bu[userID] = float(Bu[:-1])

        #load computed Alpha_u
        for line in FILE_Alpha_U:
            userID, Alpha_u = line.split(",")
            self.Alpha_u[userID] = float(Alpha_u[:-1])

        #load computed Tu
        for line in FILE_Tu:
            userID, Tu = line.split(",")
            self.Tu[userID] = float(Tu[:-1])

        #load computed Bi_Bin
        for line in FILE_Bi_Bin:
            movieID, binNum, Bi_Bin = line.split(",")
            if (currMovieID != movieID):
                currMovieID = movieID
                self.Bi_Bin[movieID] =  {} 
            self.Bi_Bin[movieID][binNum] = float(Bi_Bin[:-1])

        #load computed Pu 
        for line in FILE_Pu:
            userID, factor, Pu = line.split(",")
            if (currUserID != userID):
                currUserID = userID
                self.Pu[userID] = {}
            self.Pu[userID][factor] = float(Pu[:-1])

        #load computed Qi
        for line in FILE_Qi:
            movieID, factor, Qi = line.split(",")
            if (currMovieID != movieID):
                currMovieID = movieID
                self.Qi[movieID] = {} 
            self.Qi[movieID][factor] = float(Qi[:-1])

        #load computed sumMW
        for line in FILE_sumMW:
            userID, factor, sumMW = line.split(",")
            if (currUserID != userID):
                currUserID = userID
                self.sumMW[userID] = {} 
            self.sumMW[userID][factor] = float(sumMW[:-1])

        #load computed Bu_t
        for line in FILE_Bu_t:
            userID, day, Bu_t = line.split(",")
            if (currUserID != userID):
                currUserID = userID
                self.Bu_t[userID] = {}
            self.Bu_t[userID][day] = float(Bu_t[:-1])

        #load computed Dev
        for line in FILE_Dev:
            userID, day, Dev = line.split(",")
            if (currUserID != userID):
                currUserID = userID
                self.Dev[userID] = {} 
            self.Dev[userID][day] = float(Dev[:-1])

        #load train data size 
        for line in FILE_Sz:
            userID, Sz = line.split(",")
            self.Sz[userID] = int(Sz)

        print("Parameters Loaded")

    def callDev(self, userID, timeArg):
        if self.Dev.get(userID, {}).get(timeArg, 0) != 0:
            return self.Dev[userID][timeArg]
        else:
            tmp = sign(timeArg - self.Tu.get(userID, 0)) * pow(abs(timeArg - self.Tu.get(userID, 0)), 0.4) 
            self.Dev[userID] = {}
            self.Dev[userID][timeArg] = tmp
            return tmp

    def calBin(self, timeArg):
        binSize = CONST_dayCount/CONST_binCount + 1
        return int(timeArg/binSize)

    def predictScore(self, userID, movieID, dateArg):
        day = date2day(dateArg)
        tmp = 0.0
        sz = self.Sz.get(userID, 0)
        sqrtNum = 0
        if sz > 1:
            sqrtNum = 1/(sqrt(sz))
        for i in range(CONST_factor):
            tmp += (self.Pu.get(userID, {}).get(str(i), 0) + self.sumMW.get(userID, {}).get(str(i), 0)*sqrtNum) * self.Qi.get(movieID, {}).get(str(i), 0)
        score = CONST_avg + self.Bu.get(userID, 0) + self.Bi.get(movieID, 0) + self.Bi_Bin.get(movieID, {}).get(str(self.calBin(day)), 0) + self.Alpha_u.get(userID, 0)*self.callDev(userID, day) + self.Bu_t.get(userID, {}).get(day, 0) + tmp

        if score > 5:
            return 5
        elif score < 1:
            return 1 
        else:
            return score
    
        # return score, CONST_avg, self.Bu.get(userID, 0), self.Bi.get(movieID, 0), self.Bi_Bin.get(movieID, {}).get(str(self.calBin(day)), 0), self.Alpha_u.get(userID, 0)*self.callDev(userID, day), self.Bu_t.get(userID, {}).get(day, 0), tmp
