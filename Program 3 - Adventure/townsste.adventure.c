/*********************************************
* CS344 - Assignment 2 - Adventure
* Author: Stephen Townsend 
* File: adventure.c
* This c file is used to read data from a file
* and play the adventure game.  This will use 
* the buildroom.c to function and creates the 
* files to use.
*********************************************/

#define _CRT_SECURE_NO_WARNINGS

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>						//Unable to get multi time threading to work

char dirName[25] = "townsste.rooms.";		//Directory. Longest string possible = 15 chars - pid
char fileLoc[30];							//FilePath. Longest string possible = 22 chars
char playersPath[50];						//Keep track of the usersPath to victory
char currentFile[12];						//This is to hold the name of the file we are using
char userInput[20];							//Hold user input
char store[10];								//Store a name that we grab from the files
char line[150];								//Hold line information from the file.
FILE *fp;									//File pointer

#define NUM_THREADS 1



/*******************************************
*			removeEndNewLine
* This function is used to remove the newline
* character at the end of each string.  This
* will make outputting more uniform.
********************************************/
void removeEndNewLine(char *removeItem)
{
	removeItem[strcspn(removeItem, "\n")] = '\0';	//Change the found newline character 
}

/*******************************************
*			setFile
* This function is used to set a file directory
* from currentFile location to fileLoc.
* Now we can access the file we want.
********************************************/
void setFile()
{
	sprintf(fileLoc, "%s/%s", dirName, currentFile); //Use fileLoc to create the directory path
}



/*******************************************
*			setDirAndFirstFile
* This function is used to set the directory
* with the most current pid.  The function
* will also grab the name of the start_room
* from TempLineup.
********************************************/
void setDirAndFirstFile()
{
	int i;
	strcpy(currentFile, "TempLineup");
	sprintf(fileLoc, "%s", currentFile);			//Use fileLoc to create the directory path
	fp = fopen(fileLoc, "r");
	for (i = 0; i < 8; i++)
	{
		fgets(line, sizeof line, fp);				//Get second line onward till at last connection 
		if (i == 0)
		{
			strcpy(currentFile, line);
			removeEndNewLine(currentFile);			//Remove the newline character of string
		}
	}
	strncpy(store, line, 7);						//Copy each line to store.  14 characters in and last 7 characters
	
	strcat(dirName, store);
	fclose(fp);
	setFile();										//Reset directory path to currentFile and not currentTime
}
									
/*******************************************
*			timeToFile
* This function is used to put the time
* function into a textfile called currentTime
********************************************/
timeToFile(char *timeArray)
{
	sprintf(fileLoc, "%s", "currentTime.txt"); //Use fileLoc to create the directory path
	fp = fopen(fileLoc, "a");						//Open the current directory file in write mode 
	fprintf(fp, "%s\n", timeArray);					//Print the time array to the file
	fclose(fp);										//Close the current file

	setFile();										//Reset directory path to currentFile and not currentTime
}

/*******************************************
*			checkInput
* This function is used to check if the user
* input is equal to a file in the possible
* connection.
********************************************/
int checkInput(int lines, char *test)
{
	int i = 0;

	fp = fopen(fileLoc, "r");						//Open the current directory file in read mode 
	fgets(line, sizeof line, fp);					//Get first line in the file
	while (i < lines - 1)							//Check the lines from 2 to max - 1
	{
		fgets(line, sizeof line, fp);				//Get second line onward till at last connection 
		strncpy(store, line + 14, 7);				//Copy each line to store.  14 characters in and last 7 characters

		removeEndNewLine(store);					//Remove the newline character of string
		if (strcmp(store, test) == 0)				//Compare stored name with the user input
			return 0;								//User input is found in file as connection
		i++;										//Go to next line if name does not match
	}
	return 1;										//Could not find any matches
}

/*******************************************
*			checkEnd
* This function is used to check the current
* file and if it is the end room.
* Only returns a bool true if the room is 
* end_room
********************************************/
bool checkEnd()
{
	fp = fopen(fileLoc, "r");						//Open the current directory file in read mode
	
	while (fgets(line, sizeof line, fp) != NULL)	//Check each line of file untill it is NULL
	{
		strncpy(store, line + 11, 8);				//Copy each line into store.  11 characters in and last 8 characters
		removeEndNewLine(store);					//Remove the newline character of string
		if (strcmp(store, "END_ROOM") == 0)			//Check if stor is equal to END_ROOM
		{
			return true;							//Is equal
		}
	}
}

/*******************************************
*			timeFuncrion
* This function is get and output the current
* time, date, and day of the week.
* Reffernce for function:
* https://stackoverflow.com/questions/1442116/how-to-get-date-and-time-value-in-c-program
********************************************/
timeFunction()
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char s[64];
	strftime(s, sizeof(s), "%I:%M%p, %A, %B %d, %Y", tm);
	printf("\n%s\n", s);

	timeToFile(s);
}

/*******************************************
*					play
* This function is used to run the program.
* It will read in files and direct the program
* the the necessary functions.
********************************************/
int play()
{
	int i = 0;
	bool end = false;
	int numLines = 0;
	int totalMoves = 0;
	int j;

	while (end == false)							//While the end room is not rached
	{
		fp = fopen(fileLoc, "r");					//Open the current directory file in read mode

		while (fgets(line, sizeof line, fp) != NULL)//While not at the end of the file
		{
			numLines++;								//Count how many lines there are in file.
		}											//This is used so that we can find start and end quick.
		fclose(fp);									//Close the current file

		fp = fopen(fileLoc, "r");					//Open the current directory file in read mode
		fgets(line, sizeof line, fp);				//Read in the first line from file START_ROOM 
		strncpy(store, line + 11, 6);				//Read the first line of the first file found previously from TempLineup.  11 Characters in the last 6 characters
		removeEndNewLine(store);					//Remove the newline character of string
		printf("CURRENT LOCATION: %s", store);		//Print out the current location
		printf("\n");								//Print newline


		printf("POSSIBLE CONNECTIONS: ");			//Print the possible connections title
		while (i < numLines - 2)					//Iterate through where i < the total lines minus first and last
		{	
			fgets(line, sizeof line, fp);			//Read in connections lines
			strncpy(store, line + 14, 7);			//Copy the string to store.  Last 7 characters, 14 characters in
			removeEndNewLine(store);				//Remove the newline character of string						
			printf("%s", store);					//Print the string stored in store
			i++;

			if (i < numLines - 2)					//If the location is still in the mid_room
				printf(", ", store);				//Print comma
			else									//If at the end of mid_room
				printf(".\n", store);				//Print period
		}
		fclose(fp);									//Close the current file

		printf("WHERE TO? >");						//Print the user prompt
		scanf("%s", userInput);						//Get user input and store in array

		if (checkInput(numLines, userInput) == 0)	//Check if the user input is a real connection
		{
			strcpy(currentFile, userInput);			//Valid connection. Move to next room.
			totalMoves++;							//Increase move counter
			setFile();								//Set the new directory of the next room
			end = checkEnd();						//Check if at the end_room with true bool return
			strcat(playersPath, currentFile);		//Cat the current location to the users path
			strcat(playersPath, "\n");				//Add newline character at the end of cat for newline in output
			for (j = 0; j < 10; j++)
				store[j] = NULL;					//Clear the store array. Set to NULL
		}
		else if (strcmp(userInput, "time") == 0)	//Check if user input was time.  Unable to work in thread time check
			timeFunction();							//Call time function to handle the time
		else
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n"); //Choice not found. Outpout cannot find room

		numLines = 0;								//Reset numLines for new file
		i = 0;										//Reset i iterator
		printf("\n");								//Print a newling chartacter
	}
	return totalMoves;								//Return the users total moves made to main
}

/*******************************************
*			CleanupTemp
* This function is used to remove the
* Temp Lineup that has the rooms in order
* From start_room to mid_rooms to end_rooms
********************************************/
void cleanupTemp()
{
	int test = 0;
	strcpy(fileLoc, "TempLineup");					//Make TempLineup the current file
	remove(fileLoc);								//Remove TempLineup file
}

/*******************************************
*			threadedTime
* This function is a work in progress and 
*w would be used to get the system time when
*prompted by the user.
********************************************/
int threadedTime()
{
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int result_code, index;
	//pthread_create(threads[1], NULL, threaded(), NULL);
	return 0;
}


int main()
{
	int moves = 0;
	int k;

	setDirAndFirstFile();
	threadedTime();
	moves = play();									//Run the program
	
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");			//End room found
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", moves);	//Steps took and path

	for (k = 0; k < 50; k++)
			printf("%c", playersPath[k]);			//Print to users path

	cleanupTemp();									//Remove TempLineup file used in build to find the starting file

	exit(0);
}