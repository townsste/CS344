Write a bash shell script to compute statistics.  
The purpose is to get you familiar with the Unix shell, shell programming, 
Unix utilities, standard input, output, and error, pipelines, process ids, 
exit values, and signals (at a basic level).


Overview
In this assignment, you will write a bash shell script to calculate mean averages 
and medians of numbers that can be input to your script from either a file or via stdin. 
This is the sort of calculation I might do when figuring out the grades for this course. 
The input will have whole number values separated by tabs, and each line will have the 
same number of values (for example, each row might be the scores of a student on assignments). 
Your script should be able to calculate the mean and median across the rows (like I might do 
to calculate an individual student's course grade) or down the columns (like I might do to 
find the average score on an assignment).