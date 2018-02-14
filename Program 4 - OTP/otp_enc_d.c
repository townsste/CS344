/*********************************************
* CS344 - Assignment 4 - OTP
* Author: Stephen Townsend
* File: otp_enc_d.c
* This program is used as a server.  This is 
* designed to run in the background.  It will 
* create a socket and listen for the client. 
* So that data can be sent accross the network. 
* This will recieve a textfile and a key file 
* from otp_enc client.  The textfile will be 
* encrypted against the key.  Once the textfile 
* has been encrypted, the data will be sent 
* back to the client.
****syntax: otp_enc_d listening_port &****
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#define ARR_SIZE  80000									//Define a constant file size

/*Prototpye initialization*/
void encrypt(char[], char[], char[]);
bool verifyConnection(int);
void receiveClient(int, char[]);
void sendClient(int, char[]);
void clearBuffer(char[]);

/******************************************
*				main
* This will be used to direct the server.
* the socket and connections are created
* here.  This is where we can call send
* and recieve.
******************************************/
void main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char fileBuffer[ARR_SIZE];
	char keyBuffer[ARR_SIZE];
	char encryp[ARR_SIZE];
	struct sockaddr_in serverAddress, clientAddress;

	//Check usage & args
	if (argc < 2)
	{
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	}

	/*Set up the server address struct*/
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]);							//Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;					//Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);			//Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY;			//Any address is allowed for connection to this process

														/*Set up the socket*/
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);	//Create the socket
	if (listenSocketFD < 0)
		error("ERROR opening socket");

	/*Enable the socket to begin listening*/
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");

	/*Flip the socket on*/
	listen(listenSocketFD, 5);							//It can receive up to 5 connections 

														/*Accept a connection, blocking if one is not available until one connects*/
	while (1)
	{
		sizeOfClientInfo = sizeof(clientAddress);		//Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); //Accept

		if (establishedConnectionFD < 0)
			error("ERROR on accept");

		/*Run a new process*/
		int spawnPid;
		int childExitMethod = -2;

		spawnPid = fork();

		if (spawnPid == 0)
		{
			/*Check that server is talking with the correct client*/
			if (verifyConnection(establishedConnectionFD) == true)
			{
				//Recieve textfile from client
				receiveClient(establishedConnectionFD, fileBuffer);

				//Recieve keyfile from client
				receiveClient(establishedConnectionFD, keyBuffer);

				//Encrypt the text file with key
				clearBuffer(encryp);
				encrypt(fileBuffer, keyBuffer, encryp);

				//Send encryped data to client
				sendClient(establishedConnectionFD, encryp);

				_exit(0);
			}
		}
		else //Parent
		{
			waitpid(spawnPid, &childExitMethod, WNOHANG); //Do not need to wait for child to finish
		}
		close(establishedConnectionFD);					//Close the existing socket which is connected to the client
	}
	close(listenSocketFD);								//Close the listening socket
}

/******************************************
*				receiveClient
* This function is used to recieve data from
* the client.  It checks for the newline
* character to see if it has recieved all
* the data.
******************************************/
void receiveClient(int establishedConnectionFD, char buffer[])
{
	int charsRead;
	char tempBuffer[ARR_SIZE];							//Hold the incomming data
	clearBuffer(tempBuffer);							//Clear any trash

	while (strstr(tempBuffer, "\n") == NULL)			//Loop till newline char is found
	{
		clearBuffer(buffer);							//clear the current buffer
		charsRead = recv(establishedConnectionFD, buffer, (strlen(buffer) - 1), 0); // Read the client's message from the socket
		strcat(tempBuffer, buffer);						//Cat the incomming data to tempBuffer
		if (charsRead < 0)
			error("CLIENT: ERROR reading from socket");
	}
	strcpy(buffer, tempBuffer);							//Copy complete tempBuffer contents to buffer.
}

/******************************************
*				sendClient
* This function is used to send data to
* the client.
******************************************/
void sendClient(int establishedConnectionFD, char buffer[])
{
	int charsRead;

	charsRead = send(establishedConnectionFD, buffer, strlen(buffer), 0);
	if (charsRead < 0)
		error("ERROR writing to socket");
}

/******************************************
*				verifyConnection
* This function is used to make sure that
* the server is talking to otp_enc client
* and not otp_dec.
******************************************/
bool verifyConnection(int connectionFD)
{
	char check[4];
	int charsRead;

	/*Check clients verification code*/
	clearBuffer(check);									//Clear Buffer
	charsRead = recv(connectionFD, check, 4, 0);		//Get clients verification code
	if (charsRead < 0)
		error("ERROR reading from socket");

	if (strcmp(check, "enc") == 0)						//If code is valid
	{
		char respond[2] = "Y";

		// Send a Success message back to the client
		charsRead = send(connectionFD, respond, 1, 0);  //Send back confirmation
		if (charsRead < 0) error("ERROR writing to socket");
		return true;									//Continue
	}
	else
	{
		char respond[2] = "N";

		charsRead = send(connectionFD, respond, 1, 0);  //Send back bad confirmation
		if (charsRead < 0) error("ERROR writing to socket");
		return false;									//Cannot Continue
	}
}

/******************************************
*				encrypt
* This function is used to encrypt the
* textfile data that was sent by the client.
* This will use the key to calculate the
* crypted message.
******************************************/
void encrypt(char file[], char key[], char encryp[])
{
	int i;
	int intFile, intKey, intEncryp;

	for (i = 0; i < (strlen(file) - 1); i++)
	{	
		intFile = file[i];								//Get file element
		intKey = key[i];								//Get key element

		if (intFile == 32)								//If textfile has space
			intFile = 64;								//Use lower bound of 65 - 90

		if (intKey == 32)								//If key has space
			intKey = 91;								//Use upper bound of 65 - 90

		intFile -= 65;									//Remove the cap letter range
		intKey -= 65;									//Remove the cap letter range

		intEncryp = intFile + intKey;					//Sum text and file
		intEncryp = intEncryp % 27;						//Take the modulus of the sum by 27 chars possible
			
		intEncryp += 65;								//Bring back to capital letters

		if (intEncryp == 91 || intEncryp == 64)			//If out of range then it is a space
			intEncryp = 32;								//Set to space

		encryp[i] = intEncryp;							//Place number into char array
	}	
}

/******************************************
*				clearBuffer
* This function is used to clear the
* buffer so that it can be reused without
* having trash in it.
******************************************/
void clearBuffer(char buffer[])
{
	memset(buffer, '\0', strlen(buffer));
}