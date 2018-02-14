'''
# CS344 - Program Py - Python Exploration
# Author: Stephen Townsend 
# File: mypython.py
This program is designed to create 3 files in the same directory 
(each named differently).  Each of these 3 files contain exactly 
10 random characters from the lowercase alphabet. Each file 
contains a newline character for the 11th.  It will then read 
the files and print out on the screen the contents.  After the 
all the files were printed to the screen, it will then print out 
two random integers with a range that is (1 to 42).  Finally 
the program will also print out the product of those two numbers.
'''

import random
import sys

random.seed()                                         #Random Seed

lowerAlpha = "abcdefghijklmnopqrstuvwxyz"
number1 = 0
number2 = 0

'''
Make three folders
Write in 10 random chars, 11th is newline char
'''
for count in range(3):                                #Run three times
    f = open("random_alpha_%s" % (count + 1),"w+")     #Create random_alpha file
    for index in range(10):
        f.write(random.choice(lowerAlpha))            #Random generate alpha string   
    f.write("\n")                                     #Add newline to the end of randLine
    f.close()

'''
Print contents of files to screen
'''
for count in range(3):
    f = open("random_alpha_%s" % (count + 1),"r")     #Open random_alpha files in read mode
    print(f.read())
    f.close()

'''
Print out two random integers (1 to 42)
Print out the product of the two numbers
'''
for count in range(2):
    number1 = random.randint(1, 42)
    print(number1)
    if count == 0:                                    #Check if first loop
        number2 = number1                             #Save number 1 into 2

number1 *= number2                                    #Multiple number 1 by 2
print(number1)                                        #Print number