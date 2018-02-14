Write a simple game akin to old text adventure games like Adventure:

http://en.wikipedia.org/wiki/Colossal_Cave_AdventureLinks to an external site.

You'll write two programs that will introduce you to programming in C on UNIX 
based systems, and will get you familiar with reading and writing files. 

Overview
This assignment is split up into two programs. The first program (hereafter called 
the "rooms program") will be contained in a file named "<STUDENT ONID USERNAME>.buildrooms.c", 
which when compiled with the same name (minus the extension) and run creates a series of 
files that hold descriptions of the in-game rooms and how the rooms are connected.

The second program (hereafter called the "game") will be called "<STUDENT ONID USERNAME>.adventure.c"
 and when compiled with the same name (minus the extension) and run provides an interface for 
 playing the game using the most recently generated rooms.

In the game, the player will begin in the "starting room" and will win the game automatically 
upon entering the "ending room", which causes the game to exit, displaying the path taken by 
the player.

During the game, the player can also enter a command that returns the current time - this 
functionality utilizes mutexes and multithreading.