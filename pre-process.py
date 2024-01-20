#   written by Boonlert Randy (74297) for Group 6 TMI4033 Project

import os
import re
from datetime import date

#   init const variables
CONST_EPOCH = date(1970, 1, 1) 
CONST_TRAINING_SET_DIR = "netflix_prize_data/training_set" 

#   open or create file for writing processed data
qualifying = open("netflix_prize_data/qualifying.txt", "r") 
qualifying_output = open("timeSVD++/test_data.txt", "w")
training_output = open("timeSVD++/training_data.txt", "w")

#   convert date to number of days afte epoch
def dateToNum(rating_date):
    year, month, day = rating_date.split('-')
    t_delta = date(int(year), int(month), int(day)) - CONST_EPOCH
    return t_delta.days 

#   process qualifying data
print("processing qualifying data")

for line in qualifying:
    if(re.search(":$", line)):
        movieID = line[:-2]
        print("current movie: " + movieID)
    else:
        userID, r_date = line.split(',')
        qualifying_output.write(userID + "," + movieID + "," + str(dateToNum(r_date)) + "\n")

qualifying.close()

#   process training data
print("processing user-movie ratings data")

for file in os.listdir(CONST_TRAINING_SET_DIR):
    content = open(CONST_TRAINING_SET_DIR + "/" + file, "r")
    for line in content:
        if(re.search(":$", line)):
            movieID = line[:-2]
            print("current movie: " + movieID)
        else:
            userID, rating, r_date = line.split(',')
            training_output.write(userID + "," + movieID + "," + rating + "," + str(dateToNum(r_date)) + "\n")
    content.close()
    
training_output.close()

print("test data processed")
