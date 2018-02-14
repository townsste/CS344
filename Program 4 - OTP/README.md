Overview
In this assignment, you will be creating five small programs that encrypt and decrypt 
information using a one-time pad-like system. I believe that you will find the topic quite 
fascinating: one of your challenges will be to pull yourself away from the stories of real-world 
espionage and tradecraft that have used the techniques you will be implementing.

These programs serve as a capstone to what you have been learning in this course, and will 
combine the multi-processing code you have been learning with socket-based inter-process 
communication. Your programs will also accessible from the command line using standard 
UNIX features like input/output redirection, and job control. Finally, you will write a 
short compilation script.

Plaintext is the term for the information that you wish to encrypt and protect. It is human readable.

Ciphertext is the term for the plaintext after it has been encrypted by your programs. 
Ciphertext is not human-readable, and in fact cannot be cracked, if the OTP system is used correctly.

A Key is the random sequence of characters that will be used to convert Plaintext to 
Ciphertext, and back again. It must not be re-used, or else the encryption is in 
danger of being broken.