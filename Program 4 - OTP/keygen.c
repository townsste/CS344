/*********************************************
* CS344 - Assignment 4 - OTP
* Author: Stephen Townsend
* File: keygen.c
* This c file is used to create and encryption
* key. The key will be used in otp_enc.c, 
* otp_dec.c, otp_enc_d.c, and otp_dec_d.c.
* The key will be used against a text file.
* Once thr program is finished the key should
* destroyed.
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*******************************************
*					main
* This main function is used to generate a 
* random number based on the quantity passed
* into the program.
	   ****Syntax: keygen keylength****
********************************************/

/*
modulus 27 operations: 
:::::27 characters:::::
26 capital letters, and the space character ( ).

A starts at ascii 65
Z Ends at Ascii 90
Space is at Ascii 32

Be between 65 & 90 and allow space
*/
void main(int argc, char *argv[])
{
	srand(time(NULL));						//Seed random number
	
	int i;
	int random;

	for (i = 0; i < atoi(argv[1]); i++)
	{
		random = rand() % 27 + 65;			//Select a random room between 65-91
		if (random == 91)					//Check if 91 '['
			random = 32;					//Change to 32 'space'
		printf("%c", random); 
	}
	printf("\n");							//Place a newline char at end of file
}