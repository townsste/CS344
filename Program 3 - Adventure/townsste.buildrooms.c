/*********************************************
* CS344 - Assignment 2 - Adventure
* Author: Stephen Townsend
* File: buildrooms.c
* This c file is used to build room files that
* will be used by the adventure.c file.  The
* names of the rooms are based off of the Snow
* White dwarfs.  However, since there are only 
* seven, I had to add three of their cousins.
*********************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

char dirName[25];		 //Directory. Longest string possible = 15 chars - pid
char fileLoc[30];		 //FilePath. Longest string possible = 22 chars
char *availRooms[] = { "Doc", "Grumpy", "Happy", "Sleepy", "Bashful", "Sneezy", "Dopey", "Lovey", "Klutzy", "Lazy" };
char *rooms[7];			 //Holds the order of the rooms from start to finish
char *connections[7][7]; //Holds each rooms connections
const int MAX = 7;		 //The total amount of rooms
FILE *fp;				 //File pointer

/*******************************************
*			createDirectory
* This function is used to create a directory
* with my ONID username and the systems process
* id.
********************************************/
void createDirectory()
{
	sprintf(dirName, "townsste.rooms.%d", getpid());	//Set directory name
	mkdir(dirName, 0755);								//Set the permissions
}

/*******************************************
*				openFile
* This function is used to open a create and 
* open a file with a specific room name.
********************************************/
void openFile(int location)
{
	sprintf(fileLoc, "%s/%s", dirName, rooms[location]); //Create a new file in the directory
	fp = fopen(fileLoc, "a");							 //Open the file
}

/*******************************************
*			randomRoomGenerator
* This function is used to generate a random
* number so that the program can use a random 
* room durring the connections process.
********************************************/
void randomRoomGenerator(int loc)
{
	int random;									//Int place holder for random number

	do
	{
		random = rand() % 10;					//Select a random room between 0-9
	} while (availRooms[random] == NULL);		//Check if the room is NULL (Unavailiable)

	rooms[loc] = availRooms[random];			//Set the rooms location to a availiable room at that location
	availRooms[random] = 0;						//The room is no longer availiable to use
}

/*******************************************
*			roomToFile
* This function is used to print the room 
* name to its specific file.
********************************************/
void roomToFiles()
{
	int i = 0;

	while (i < MAX)								  //While we are not at the MAX
	{
		randomRoomGenerator(i);				      //Get a random room
		openFile(i);							  //Open the rooms file
		fprintf(fp, "ROOM NAME: %s\n", rooms[i]); //Add the room name to the file
		fclose(fp);								  //Close the current file
		i++;									  //Iterate to next room
	}
}

/*******************************************
*			connectionsToFile
* This function is used to generate the
* connections and print them to their 
* respectable files.
********************************************/
void connectionsToFile()
{
	int i;
	int j;

	for (i = 0; i < MAX; i++)
	{
		openFile(i);								//Open the room file

		for (j = 0; j < MAX; j++)
			if (connections[i][j] != NULL)			//Check if the connections location is NULL
			{
				fprintf(fp, "CONNECTION %d: %s\n", j + 1, connections[i][j]);  //Print connection to room file
			}
		fclose(fp);									//Close the current file
	}
}

/*******************************************
*			roomTypeToFile
* This function is used to print the room type
* the the correct room file.
* Either START_ROOM, MID_ROOM, or END_ROOM.
********************************************/
void roomTypeToFile()
{
	int i;
	for (i = 0; i < MAX; i++)
	{
		openFile(i);							//Open the room file

		fprintf(fp, "ROOM TYPE: ");

		if (i == 0) 							//First File
			fprintf(fp, "START_ROOM\n");		//START_ROOM to file
		else if (i == 6)						//Seventh File
			fprintf(fp, "END_ROOM\n");			//END_ROOM to file
		else									//Second through sixth file
			fprintf(fp, "MID_ROOM\n");			//MID_ROOM to file

		fclose(fp);								//Close the current file
	}
}

/*******************************************
*			checkConnections
* This function is used to check and make
* sure that the connections is not a duplicate
* Or used before.
********************************************/
bool checkConnections(int loc, char test[15])
{
	int i;

	for (i = 0; i < MAX; i++)
	{
		if (connections[loc][i] != NULL)				//Check if current location is NULL
		{
			if (strcmp(connections[loc][i], test) == 0) //Compare the location with the test name
				return true;							//The name is already in the list
		}
	}
	return false;										//The name has not been used yet (Valid)
}

/*******************************************
*			makeRoomConnections
* This function is used to connect each of 
* the rooms together.  It will generate a 
* random number to determine the amount of 
* connections to be made.  There is a check
* on the availiable connections that can be 
* made.  If there are only three connections 
* availiable then the function will move on.
* The minimum connections is three and the
* maximum of outbound connections are siz.
********************************************/
void makeRoomConnections()
{
	int random = 0;
	int connectionNumber = 0;
	int count = 0;
	int availSpaces = 0;
	int validRandom = 1;
	int i;
	int j;

	for (i = 0; i < MAX; i++)
	{
		while (count < MAX)								//Check how many empty spaces there are
		{
			if (connections[i][count] == NULL)			//If location is Null it is free
				availSpaces++;							//Add one to space counter
			count++;									//Move to next element
		}

		if (availSpaces > 3)							//If there are more then 3 spaces then fill
		{
			do
				connectionNumber = rand() % 3 + 3;		//Select a random connection amount from 3-6
			while (connectionNumber >= availSpaces);	//Keep doing this untill the number is good.

			for (j = 0; j < connectionNumber; j++)
			{
				while (validRandom == 1)				//Make sure the room is valid		
				{
					count = 0;							
					random = rand() % MAX;				//Select a random room between 0-6

					if (checkConnections(i, rooms[random]) == false) //Check if number was used yet
					{
						if (random != i)				//Make sure room is not current room
							validRandom = 0;			//Valid room is found
					}
				}
				connections[i][j] = rooms[random];		//Set the room into the connection

				if (checkConnections(random, rooms[i]) == false)	//Check the connection
				{
					int increment = 0;
					while (connections[random][increment] != NULL)	//Get the last location in the matrix array
					{
						increment++;
					}

					connections[random][increment] = rooms[i];		//Set this rooms location to the random room aswell
				}
				validRandom = 1;						//Reset validation number;
			}
		}
		availSpaces = 0;								//Reset availiable spaces for next room
		count = 0;										//Reset counter
	}
}

/*******************************************
*				tempLineup
* This function is used to create a temporary
* lineup that will be used in the adventure 
* code.  This will make it easier to find the
* starting and end rooms.
********************************************/
void tempLineup()
{
	int i;

	sprintf(fileLoc,"%s", "TempLineup");				//Create the TempLineup file in current directory
	fp = fopen(fileLoc, "w");							//Open the file with write permissions

	for (i = 0; i < MAX; i++)
		fprintf(fp, "%s\n", rooms[i]);					//Print each room in order to the file

	for (i = 15; i < 25; i++)
		if (dirName[i] != NULL)							//If dirName is not NULL
			fprintf(fp, "%c", dirName[i]);				//Print the pid in directory name

	fclose(fp);											//Close the current file
}


int main()
{
	srand(time(NULL));									//Seed random number
	
	//Function Calls to run the buildings
	createDirectory();
	roomToFiles();
	makeRoomConnections();
	connectionsToFile();
	roomTypeToFile();
	tempLineup();

	exit(0);
}