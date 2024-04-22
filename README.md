# capstone
2023/2024 HPU Capstone - Chess Education Robot

This is the code space for the High Point University Senior Electrical & Computer Engineering Capstone
Project for 2024. The code uses I2C communication between 3 Arduino MEGAs to mangage a SCARA (robotic arm),
Capactive Touch input system, sub-board LEDs for illuminating hints, a LCD Touchscreen (currently semi-functional),
and the Micro-Max Chess Engine. 

3 system files:
1) FullBoard_w_Rules
2) SCARA_Final
3) main (currently using UserBestMove with 16x2 LCD w/ Keypad Shield)

The original code, "Chessuino" was a ported chess program to Arduino MEGA by Diego Cueva (June 2014). 
Based on Micro-Max, version 4.8, a chess program smaller than 2KB (of non-blank source), by H.G. Muller. 
Port to Atmel ATMega644 and AVR GCC, by Andre Adrian 

As of graduation, the system was about 75-80% functional. The SCARA, Capacitive Touch input, and LED hint output
systems are all fully functional. The wiring inside the board does make the system sensitive however, but a full
system power cycle can fix most issues. Note: there is no game memory, so power loss resets the game.

The main issues plauging the system were the integration of the LCD Touchscreen and the chess engine as when 
they were running on the same device thechess engine function ('D') would not calculate correctly. 
The LCD code itself is complete, as well as the code for all the other systems minus the full promotion handling 
between the capacitive touch and chess engine. The code structure is there, but has not been fully implemented and tested.
