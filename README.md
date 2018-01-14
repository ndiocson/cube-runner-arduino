
# CMPUT 274 - FINAL PROJECT: CUBE RUNNER

- NICK DIOCSON (1492649)
- COLE MILBURN (1515586)


# Project Description

Our final project is based around the on-line game known as "Cube Field" or "Cube Runner". This is a
3D arcade-like game in which the player controls a triangular cursor accelerating through a field of
cube-like objects. The objective of the game is very simple; to avoid colliding with an oncoming cube
for as long as possible. While the player is alive, the difficulty will increase with the game spawning
more blocks to clog the player's available pathways. As the game progresses, the player will rack up
points based on the number of cubes they have passed. In addition to this, the player is also able to
view their score relative to the next highest score(s) on the current leader board while playing the
game. This adds a competitive edge to the game, allowing the player to face off against their friends.


# Game Instructions

To play the game is very simple. Using the Arduino's joystick, the player is able to move left or right
relative the the direction they push in. Should the player find the need to pause while in the middle of
a game, the player may click the joystick button to freeze time. To get back to the game, simply click the
button a second time. If a high score is achieved, once the player dies they will be prompted by the serial
monitor to enter in their name. This will be stored onto their SD card along with their score. Note that only
letters should be used when storing names, and capital letters are ALREADY implemented so there is no need to
use the shift or caps-lock keys. Should an SD card not be available to the player, the game will still be able
to be played just without the functionality of the leader board ad the "TO BEAT" score tracker.


# Wring Instruction

The wiring of this game is exactly the same as other projects and assignments using the joystick and screen,
(namely Assignment 2 Parts 1 & 2), however a guide will still be provided:

Adafruit ILI9341
- GND  <--> GND
- Vin  <--> +5V
- CLK  <--> 52
- MISO <--> 50
- MOSI <--> 51
- CS 	 <--> 10
- D/C  <--> 9
- Y+ 	 <--> A2 (analog pin)
- X+   <--> 4  (digital pin)
- Y-   <--> 5  (digital pin)
- X-   <--> A3 (analog pin)
- CCS  <--> 6

NOTE: The rest of the pins are NOT to be connected to the Arduino Board

SainSmart Joystick
- GND <--> GND
- +5V <--> +5V
- VRx	<--> A1 (analog pin)
- VRy <--> A0 (analog pin)
- SW <--> 2 (digital pin)


# Miscellaneous

This project does not require any additional parts other than the Adafruit screen and the SainSmart Joystick.

Some functions, such as swap(), pivot() and qsort(), were used from previous assignment(s) to be implemented
within this current code.
