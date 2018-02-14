/*********************************************
* CS344 - Assignment 4 - OTP
* Author: Stephen Townsend
* File: otp_enc.c
* This program is used as a client.  This will
* send a textfile and a key file to the 
* otp_enc_d server to encrypt the textfiles 
* data.  This program will attempt to open a
* file and verify its contents.  Once everything
* is in accordance, then the text file and key 
* wil be sent to the server.  Once the Server
* has encrpted the textfile, it will send the
* encrypted data back here where it will be 
* displayed or outputted to a new file.
****syntax: otp_enc plaintext key port****
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define ARR_SIZE  80000									//Define a constant file size

/*Prototpye initialization*/
void checkFile(char *[]);
void sendFile(int, char [], char *[]);
void sendServer(int, char[]);
void sendServer(int, char[]);
void receiveServer(int, char []);
void clearBuffer(char []);


/******************************************
*				main
* This will be used to direct the client.
* the socket and connections are created 
* here.  This is where we can call send
* and recieve.
******************************************/
void main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[ARR_SIZE];

	/*Contents of argv[argc]*/
	//argv[0] = Current Program
	//argv[1] = Textfile Name
	//argv[2] = Keyfile Name
	//argv[3] = Port Number

	checkFile(argv);									//Check to make sure the files are acceptable

	/*Check usage & args*/
	if (argc < 3) 
	{ 
		fprintf(stderr, "USAGE: %s hostname port\n", argv[0]); 
		exit(0); 
	} 
	
	/*Set up the client address struct*/
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //Clear out the address struct
	portNumber = atoi(argv[3]);							//Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;					//Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);			//Store the port number
	serverHostInfo = gethostbyname("localhost");		//Convert the machine name into a special form of address
	if (serverHostInfo == NULL) 
	{ 
		fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
		exit(0); 
	}
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	/*Set up the socket*/
	socketFD = socket(AF_INET, SOCK_STREAM, 0);			//Create the socket
	if (socketFD < 0) 
	{
		fprintf(stderr, "Error: could not contact otp_enc_d on port %d\n", portNumber);
		exit(2);										//Exit with error
	}

	/*Connect to server*/
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	/*Confirmation message*/
	char verifMessage[4] = "enc";
	sendServer(socketFD, verifMessage);					//Send message to server
	receiveServer(socketFD, verifMessage);				//Receive message from server

	if (strcmp(verifMessage, "Y") == 0)
	{
		/*Send plaintext to server*/
		sendFile(1, buffer, argv);						//Prep file to send
		sendServer(socketFD, buffer);					//Send message to server

		/*Send key to server*/
		sendFile(2, buffer, argv);						//Prep file to send
		sendServer(socketFD, buffer);					//Send message to server

		/*Get cyphertext from server*/
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, buffer, (sizeof(buffer) - 1), 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0)
			error("CLIENT: ERROR reading from socket");

		fprintf(stdout,"%s\n", buffer);					//Print to stdout cyphertext
	}
	else
	{
		/*Not Verified.  Cannot connect to dec*/
		fprintf(stderr, "otp_enc cannot use otp_dec_d. The connection was rejected");
		exit(1);										//Exit with error
	}
		
	close(socketFD);									//Close the socket
	exit(0);											//Exit no problems
}

/******************************************
*				checkFile
* This function is used to read and check 
* the files contents.  If the file had bad 
* characters then an error will prompt and
* will exit.  If the Key > textfile then 
* an error will prompt and exit.
******************************************/
void checkFile(char *argv[])
{
	/*If otp_enc receives key or plaintext files with bad characters in them,
	or the key file is shorter than the plaintext, it should exit with an error
	and set the exit value to 1.*/
	
	int i;
	int check = 1;										//Check both Textfile and Key
	int keySize;
	int textfileSize;

	while (check != 3)
	{
		int test;
		char *inputBuffer;

		//1 == textfile
		//2 == key
		int fd = open(argv[check], O_RDONLY);			//Open a specific file

		int size = lseek(fd, 0, SEEK_END);				//Get the size of the file
		lseek(fd, 0, 0);								//Return back to the beginning of the file

		inputBuffer = malloc(size);						//Dynamically Create array to file size

		if (check == 1)
			textfileSize = size;						//Keep plaintext size
		else
			keySize = size;								//Keep key size

		if (read(fd, inputBuffer, size) < 0)
		{
			fprintf(stderr, "An error occurred while reading.\n");
			free(inputBuffer);							//Free memory array
			exit(1);	
		}
		else
		{
			/*Test each element to make sure they are valid*/
			for (i = 0; i < size; i++)
			{
				test = inputBuffer[i];

				if (test < 65 && test < 90 && test != 32 && test != 10)	  //Cap letters, space, or newline	
				{
					fprintf(stderr, "otp_enc error: %s contains bad characters\n", argv[1]);
					free(inputBuffer);					//Free memory array
					exit(1);
				}
			}
		}
		close(fd);										//Close file
		check++;										//Go to the next file
		clearBuffer(inputBuffer);						//Clear Buffer
		free(inputBuffer);								//Free memory array
	}

	/*Check if key is less than textfile*/
	if (keySize < textfileSize)						
	{
		fprintf(stderr, "Error: key %s is too short\n", argv[2]);
		exit(1);
	}
}

/******************************************
*				sendFile
* This function is used to open a given file
* by passing in the number and then it will
* place the contents of the file into the
* buffer that can then be sent to the server
* via the sendServer call.
******************************************/
void sendFile(int file, char buffer[], char *argv[])
{
	clearBuffer(buffer);
	int fd = open(argv[file], O_RDONLY);				//Open a specific file

	int size = lseek(fd, 0, SEEK_END);					//Get the size of the file
	lseek(fd, 0, 0);									//Return back to the beginning of the file

	if (read(fd, buffer, size) < 0)						//Check if file is readable
	{
		fprintf(stderr, "An error occurred while reading.\n");
		exit(1);										//Exit with error
	}
	close(fd);											//Close the file
}

/******************************************
*				sendServer
* This function is used send data to
* the server.
******************************************/
void sendServer(int socketFD, char buffer[])
{
	int charsWritten;

	charsWritten = send(socketFD, buffer, strlen(buffer), 0);
	if (charsWritten < 0)
		error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer))
		printf("CLIENT: WARNING: Not all data written to socket!\n");
}

/******************************************
*				receiveServer
* This function is used recieve data from
* the server.
******************************************/
void receiveServer(int socketFD, char buffer[])
{
	int charsRead;

	clearBuffer(buffer);								//Clear out the buffer
	charsRead = recv(socketFD, buffer, (sizeof(buffer) - 1), 0);	//Read data from the socket, leaving \0 at end
	if (charsRead < 0)
		error("CLIENT: ERROR reading from socket");
}

/******************************************
*				clearBuffer
* This function is used to clear the
* buffer so that it can be reused without
* having trash in it.
******************************************/
void clearBuffer(char buffer[])
{
	memset(buffer, '\0', sizeof(buffer));
}