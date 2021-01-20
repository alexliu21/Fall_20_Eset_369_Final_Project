# Fall_20_Eset_369_Final_Project
Final project for ESET 369 at Texas A&M University, Fall 2020
By Alex Liu

Components List:
 - NI MSP432P401R Microcontroller
 - BH EDU Board
 - Servo Motor
 - External battery pack
 - Jumper Wires (F-F and M-F)

This project was written in C and used Code Composer Studio as an IDE. It consisted of a physical component involving a National Instruments MSP432P401R and a BH EDU board, created by Professor Byul Hur, and a digital pacman robot and digital BH EDU board that would display in the included executable. 
Using the directional buttons on the BH EDU board, the digital pacman could be navigated to the goal, where a press of SW1 on the microcontroller would cause the pacman to close its mouth and yield a "win" state.  Directions would be displayed in real time on the physical BH EDU's LCD module e.g. a right button input would cause "Right" to display. The second row of the LCD module was reserved for the creator's name and remained static throughout. Pressing SW1 not only caused the pacman to close its mouth, but also moved an attached servo motor to move 90 degrees from its starting position and return after a delay.
The microcontroller's built-in accelerometer was also used in conjunction with the built-in ADC module to convert the analog values into voltage. Channels 11, 12, and 13 were used to this end. The three values were displayed on the digital BH EDU board's LCD module.
