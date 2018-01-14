/*
CMPUT 274 FINAL PROJECT
Cube Runner

Creators:
	Nick Diocson (1492649)
	Cole Milburn (1515586)

The functions (qsort, pivot, and swap) were provided in CMPUT 274 lectures

Description:
	Cube Runner is a 3D arcade-like game based off the popular game "Cube Field".
	The player controls a ship by using a joystick in order to naviagte past oncoming objects.
	The goal is to survive as long as possible by not colliding into any of the randomly generated
	cubes. As the player progresses through the game, they reach new levels which corresponds
	to an additional object being introduced into the game as well as a new color scheme. If the player sets
	a new highscore, they are prompted to type there name into the serial monitor which will be saved along
	with their highscore to the SD card. These highscores can be viewed from the main menu under Leaderboards.
*/

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include <SPI.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

#define JOY_VERT  A1 // VRx
#define JOY_HORIZ A0 // VRy
#define JOY_SEL   2

#define JOY_CENTER   512
#define JOY_DEADZONE 64

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

const int analogPin = 4;

int cursorX = 0, cursorXOld, ShipDir = 0, ShipDirOld;
int yPos1, yPos2, yPos3, yPos4, yPos5, yPos6, yPos7;
int SendObject2, SendObject3, SendObject4, SendObject5, SendObject6, SendObject7 = 0;
int dir1, dir2, dir3, dir4, dir5, dir6, dir7 = 0;
int points, spawn, spawnOld, levelind, i, beatind;
int color = ILI9341_WHITE, highlightedString = 2;
float objectX1, objectX2, objectX3, objectX4, objectX5, objectX6, objectX7 = 0;
float objectXOld1, objectXOld2, objectXOld3, objectXOld4, objectXOld5, objectXOld6, objectXOld7 = 0;

File file;

struct highScore {
	int score; // 12 bits / 3 bytes
	String name; // 16 bits / 4 bytes
} data[10];

// drawBackground() draws grid lines on the screen that are responsible fora 3d effect
void drawBackground() {

	tft.drawLine(125, 320, 125, 0, tft.color565(0,25,0));
	tft.drawLine(130, 320, 130, 0, tft.color565(0,45,0));
	tft.drawLine(135, 320, 135, 0, tft.color565(0,65,0));
	tft.drawLine(145, 320, 145, 0, tft.color565(0,85,0));
	tft.drawLine(155, 320, 155, 0, tft.color565(0,105,0));
	tft.drawLine(165, 320, 165, 0, tft.color565(0,125,0));
	tft.drawLine(180, 320, 180, 0, tft.color565(0,145,0));
	tft.drawLine(195, 320, 195, 0, tft.color565(0,165,0));
	tft.drawLine(210, 320, 210, 0, tft.color565(0,185,0));
	tft.drawLine(225, 320, 225, 0, tft.color565(0,205,0));
}

// drawCursorOld() draws over the previous cursor position with black
void drawCursorOld()	{

	// draws over the old middle position of the cursor
	tft.drawLine(210, 160 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawLine(230, 170 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawLine(230, 150 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawTriangle(230, 170 + cursorXOld, 210, 160 + cursorXOld, 230, 150 + cursorXOld, ILI9341_BLACK);
	// draws over the old left position of the cursor
	tft.drawLine(210, 160 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawLine(230 + 3, 170 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawLine(230 - 3, 150 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawTriangle(230 + 3, 170 + cursorXOld, 210, 160 + cursorXOld, 230 - 3, 150 + cursorXOld, ILI9341_BLACK);
	// draws over the old right position of the cursor
	tft.drawLine(210, 160 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawLine(230 -3, 170 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawLine(230 +3, 150 + cursorXOld, 220, 160 + cursorXOld, ILI9341_BLACK);
	tft.drawTriangle(230 -3, 170 + cursorXOld, 210, 160 + cursorXOld, 230 +3, 150 + cursorXOld, ILI9341_BLACK);
}

// drawCursor() draws the new cursor depending on direction as well as covers up the old cursor if necessary
void drawCursor() {

	// if the ShipDir is left, draw the Ship with a left tilt
	if ( ShipDir == 1)	{
		// if the cursor has moved positions, cover up the old cursor
		if (cursorX != cursorXOld) {
			drawCursorOld();
		}
		tft.drawLine(210, 160 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawLine(230 + 3, 170 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawLine(230 - 3, 150 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawTriangle(230 + 3, 170 + cursorX, 210, 160 + cursorX, 230 - 3, 150 + cursorX, tft.color565(200,200,200));
	}
	// if the ShipDir is right, draw the Ship with a right tilt
	else if ( ShipDir == -1)	{
		// if the cursor has moved positions, cover up the old cursor
		if (cursorX != cursorXOld) {
			drawCursorOld();
		}
		tft.drawLine(210, 160 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawLine(230 -3, 170 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawLine(230 +3, 150 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawTriangle(230 -3, 170 + cursorX, 210, 160 + cursorX, 230 +3, 150 + cursorX, tft.color565(200,200,200));
	}
	// if the ShipDir is straight, draw the Ship with no tilt
	else {
		// if the cursor has moved positions, cover up the old cursor
		if (ShipDirOld != ShipDir)	{
			drawCursorOld();
		}
		tft.drawLine(210, 160 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawLine(230, 170 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawLine(230, 150 + cursorX, 220, 160 + cursorX, tft.color565(200,200,200));
		tft.drawTriangle(230, 170 + cursorX, 210, 160 + cursorX, 230, 150 + cursorX, tft.color565(200,200,200));
	}
}

// initializeSD() ensures the SD card is ready for use, serial prints updates as well
// if the SD card is not available for initialization, the game may still be played without
// the leaderboard saving functionality
void initializeSD() {

	Serial.print("Initializing SD card...");

	if (SD.begin(SD_CS)) {
		Serial.println("SD card is ready to use.");
	} else {
		Serial.println("SD card initialization failed");
		return;
	}
}

// swap() is responsible for swapping two given values
void swap(highScore *ptr_score1, highScore *ptr_score2) {

	highScore tmp = *ptr_score1;
	*ptr_score1 = *ptr_score2;
	*ptr_score2 = tmp;
}

// pivot() goes through the inputed array and swaps values inside the array
int pivot(highScore *array, int n, int pi) {

	int lo = 0;
	int hi = n - 2;

	swap(&array[pi], &array[n-1]);

	while (lo <= hi) {

		if (array[lo].score <= array[n-1].score) {
			lo++;
		}
		else if (array[hi].score > array[n-1].score) {
			hi--;
		} else {
			swap(&array[lo], &array[hi]);
		}
	}

	swap(&array[lo], &array[n-1]);

	return lo;
}

// qsort() sorts an array with n elements (quick sort)
void qsort(highScore *array, int n) {
	// if n <= 1 do nothing (just return)
	if (n <= 1) {
		return;
	}
	// pick a pivot index pi
	int pi = n/2;

	// call pivot with this pivot index, store the returned
	// location of the pivot in a new variable
	int newPi = pivot(array, n, pi);

	// recursively call qsort twice:
	// - once with the part before index newPi
	qsort(array, newPi);
	// - once with the part after index newPi
	qsort(array + newPi, n - newPi);
}

// getHighScores() reads from the SD card to save the current highscores
void getHighScores() {

	char ch;
	int index = 0;
	int counter = 0;
	data[index].score = 0;
	data[index].name = "";

	while (file.available() && index <= 10) {
		ch = file.read();
		counter += 1;
		// stores the first 3 characters as the players score
		if (counter >= 1 && counter <= 3) {
			data[index].score += ceil((int) (ch - '0')*pow(10, 3 - counter));
		}
		// stores the next 4 characters as the players name
		else if (counter > 3 && counter <= 7) {
			data[index].name += ch;
		}
		if (ch == '\n') {
			index += 1;
			counter = 0;
			data[index].score = 0;
			data[index].name = "";
		}
	}
	// sorts the recieved data from lowest to highest score
	qsort(data, 10);
}

// setup() initializes the SD card and joystick pinmode as well as sets the background
void setup() {

	init();
	Serial.begin(9600);
	pinMode(JOY_SEL, INPUT_PULLUP);
	tft.begin();
	tft.fillScreen(ILI9341_BLACK);

	// resets any stored score values on the arduino
	for (int i = 0; i < 10; i++) {
		data[i].score = 0;
		data[i].name = '\0';
	}

	// if the file "SCORES.TXT" is not found within the SD card, the program will create the file and continue
	// to save to it
	initializeSD();
	Serial.print("Opening SCORES.TXT...");
	file = SD.open("SCORES.TXT", FILE_WRITE);
	if (file) {
		Serial.println("File open succsesful!");
		file.close();
	}	else {
		Serial.println("File create unsuccsesful. Leaderboard access may not work...");
		file.close();
		return;
	}

	Serial.println();
	drawBackground();
}

// toBeat() prints the next score to beat from the leaderboard to the top right corner of the screen
// by tracking the player's current number of points and comparing it to the scores found on the leaderboard
void toBeat() {

	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.setRotation(3);
	tft.setTextSize(1);
	tft.setCursor(260,5);

	if (points < data[5+i].score)	{
		tft.print("BEAT: ");
		tft.print(data[5+i].score);
	}
	else if (points >= data[5+i].score && points < data[9].score) {
		tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
		tft.setCursor(260,5);
		tft.print("BEAT: ");
		tft.print(data[5+i].score);
		i += 1;
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
		tft.setCursor(260,5);
		tft.print("BEAT: ");
		tft.print(data[5+i].score);
	}
	beatind = 0;
	tft.setRotation(4);
}

// level() prints the current level the player is on to the top left corner of the screen
void level()	{

	levelind += 1;
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.setRotation(3);
	tft.setTextSize(1);
	tft.setCursor(0,5);

	if (points == 0)	{
		tft.print("LEVEL 1");
	}
	else if (points == 25)	{
		tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
		tft.print("LEVEL 1");
		tft.setCursor(0,5);
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
		tft.print("LEVEL 2");
	}
	else if (points == 50)	{
		tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
		tft.print("LEVEL 2");
		tft.setCursor(0,5);
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
		tft.print("LEVEL 3");
	}
	else if (points == 100)	{
		tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
		tft.print("LEVEL 3");
		tft.setCursor(0,5);
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
		tft.print("LEVEL 4");
	}
	else if (points == 200)	{
		tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
		tft.print("LEVEL 4");
		tft.setCursor(0,5);
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
		tft.print("LEVEL 5");
	}
	else if (points == 400)	{
		tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
		tft.print("LEVEL 5");
		tft.setCursor(0,5);
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
		tft.print("MAX LEVEL");
	}
	tft.setRotation(4);
}

// drawObject() is responsible for drawing an oncoming object as well as choosing its spawn position
// it will draw the square objects moving down the screen while simultaneously increasing their widths
// and heights to mimick an 'incoming 3d effect'
void drawObject(int &yPos, float &objectX, float &objectXOld, int &dir) {

	// draw the object to come straight down the screen
	if (yPos > 150 && yPos < 170) {
		tft.drawRect(120 + objectX, yPos - objectX/4, objectX/2, objectX/2, color);
		tft.drawRect(120 + objectXOld, yPos - objectXOld/4, objectXOld/2, objectXOld/2, ILI9341_BLACK);
		dir = 3;
	}
	// draw the object to crawl left down the screen
	else if (yPos >= 170)	{
		tft.drawRect(120 + objectX, yPos + objectX/2, objectX/2, objectX/2, color);
		tft.drawRect(120 + objectXOld, yPos + objectXOld/2, objectXOld/2, objectXOld/2, ILI9341_BLACK);
		dir = 2;
	}
	// draw the object to crawl right down the screen
	else {
		tft.drawRect(120 + objectX, yPos - objectX, objectX/2, objectX/2, color);
		tft.drawRect(120 + objectXOld, yPos - objectXOld, objectXOld/2, objectXOld/2, ILI9341_BLACK);
		dir = 1;
	}

	objectXOld = objectX;
	objectX += 1.5;

	// erase the object once it has passed the cursor
	if (120 + objectX + objectX/2 >= 235) {
		points += 1;
		if (dir == 3)	{
			tft.drawRect(120 + objectXOld, yPos - objectXOld/4, objectXOld/2, objectXOld/2, ILI9341_BLACK);
		}
		else if (dir == 2)	{
			tft.drawRect(120 + objectXOld, yPos + objectXOld/2, objectXOld/2, objectXOld/2, ILI9341_BLACK);
		}
		else if (dir == 1)	{
			tft.drawRect(120 + objectXOld, yPos - objectXOld, objectXOld/2, objectXOld/2, ILI9341_BLACK);
		}

		// gets a random number between 0 and 4
		spawn = analogRead(analogPin) % 5;
		// if the spawn has not changed, shift over to the next spawn location
		if (spawn == spawnOld)	{
			spawn = (spawn + 1) % 5;
		}
		// assigns a y position for each spawn from 0-4
		if (spawn == 0)	{
			yPos = 230;
		}
		else if (spawn == 1)	{
			yPos = 180;
		}
		else if (spawn == 2)	{
			yPos = 160;
		}
		else if (spawn == 3)	{
			yPos = 140;
		}
		else if (spawn == 4)	{
			yPos = 90;
		}

		spawnOld = spawn;
		objectX = 0;
		objectXOld = 0;
	}
}

// mainGameplay() is responsible for recursivley drawing the first object, displaying the current level,
// assigning object color, as well as introducing extra objects
void mainGameplay() {

	// calls and updates level() depnding on the current games points
	if ((points == 0) && (levelind == 0))	{
		level();
	}
	else if ((points == 25) && (levelind == 1))	{
		level();
	}
	else if ((points == 50) && (levelind == 2))	{
		level();
	}
	else if ((points == 100) && (levelind == 3))	{
		level();
	}
	else if ((points == 200) && (levelind == 4))	{
		level();
	}
	else if ((points == 400) && (levelind == 5))	{
		level();
	}

	// checks the initial yPos of the square at the horizon; if it is beyond y = 170, let the object crawl left
	// if it is below 150, let the objectc crawl right, if it is between 170 and 150, draw the object straight down
	if (yPos1 > 150 && yPos1 < 170) {
		tft.drawRect(120 + objectX1, yPos1 - objectX1/4, objectX1/2, objectX1/2, color);
		tft.drawRect(120 + objectXOld1, yPos1 - objectXOld1/4, objectXOld1/2, objectXOld1/2, ILI9341_BLACK);
		dir1 = 3;
	}
	else if (yPos1 >= 170)	{
		tft.drawRect(120 + objectX1, yPos1 + objectX1/2, objectX1/2, objectX1/2, color);
		tft.drawRect(120 + objectXOld1, yPos1 + objectXOld1/2, objectXOld1/2, objectXOld1/2, ILI9341_BLACK);
		dir1 = 2;
	}
	else {
		tft.drawRect(120 + objectX1, yPos1 - objectX1, objectX1/2, objectX1/2, color);
		tft.drawRect(120 + objectXOld1, yPos1 - objectXOld1, objectXOld1/2, objectXOld1/2, ILI9341_BLACK);
		dir1 = 1;
	}

	// assigns a color for the blocks depending on the players score, this helps indicate different levels
	if (25 <= points && points < 50)	{
		color = ILI9341_ORANGE;
	}
	else if (50 <= points && points < 100)	{
		color = ILI9341_CYAN;
	}
	else if (100 <= points && points < 200)	{
		color = ILI9341_YELLOW;
	}
	else if (200 <= points && points < 400)	{
		color = ILI9341_PINK;
	}
	else if (300 <= points)	{
		color = ILI9341_BLUE;
	}

	// draws other objectcs depending on the players score, the higher the score, the more objects
	// to spwan to the screen
	if (objectX1 >= 48)	{
		SendObject2 = 1;
	}
	if (SendObject2 == 1)	{
		drawObject(yPos2, objectX2, objectXOld2, dir2);
	}
	if ((objectX1 >= 24) && (points >= 25))	{
		SendObject3 = 1;
	}
	if (SendObject3 == 1)	{
		drawObject(yPos3, objectX3, objectXOld3, dir3);
	}
	if ((objectX1 >= 60) && (points >= 50))	{
		SendObject4 = 1;
	}
	if (SendObject4 == 1)	{
		drawObject(yPos4, objectX4, objectXOld4, dir4);
	}
	if ((objectX1 >= 10) && (points >= 100))	{
		SendObject5 = 1;
	}
	if (SendObject5 == 1)	{
		drawObject(yPos5, objectX5, objectXOld5, dir5);
	}
	if ((objectX1 >= 70) && (points >= 200)) {
		SendObject6 = 1;
	}
	if (SendObject6 == 1) {
		drawObject(yPos6, objectX6, objectXOld6, dir6);
	}
	if ((objectX1 >= 10) && (points >= 400))	{
		SendObject7 = 1;
	}
	if (SendObject7 == 1)	{
		drawObject(yPos7, objectX7, objectXOld7, dir7);
	}

	drawBackground();
	objectXOld1 = objectX1;
	objectX1 += 1.5;

	// erases the object once it has passed the cursor
	if (120 + objectX1 + objectX1/2 >= 235) {
		points += 1;
		if (dir1 == 3)	{
			tft.drawRect(120 + objectXOld1, yPos1 - objectXOld1/4, objectXOld1/2, objectXOld1/2, ILI9341_BLACK);
		}
		else if (dir1 == 2)	{
			tft.drawRect(120 + objectXOld1, yPos1 + objectXOld1/2, objectXOld1/2, objectXOld1/2, ILI9341_BLACK);
		}
		else if (dir1 == 1)	{
			tft.drawRect(120 + objectXOld1, yPos1 - objectXOld1, objectXOld1/2, objectXOld1/2, ILI9341_BLACK);
		}

		// gets a random number between 0 and 4
		spawn = analogRead(analogPin) % 5;
		// if the spawn has not changed, shift over to the next spawn location
		if (spawn == spawnOld)	{
			spawn = (spawn + 1) % 5;
		}
		// assigns a y position for each spawn from 0-4
		if (spawn == 0)	{
			yPos1 = 230;
		}
		else if (spawn == 1)	{
			yPos1 = 180;
		}
		else if (spawn == 2)	{
			yPos1 = 160;
		}
		else if (spawn == 3)	{
			yPos1 = 140;
		}
		else if (spawn == 4)	{
			yPos1 = 90;
		}
		spawnOld = spawn;
		objectX1 = 0;
		objectXOld1 = 0;

		// recursively calls this function which calls the previous drawObject() functions with updtated poisitonal values
		mainGameplay();
	}
}

// pause() freezes the current gameplay until the joystick is pressed
void pause()	{

	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.setTextWrap(false);
	tft.setRotation(3);
	tft.setTextSize(3);
	tft.setCursor(115,50);
	tft.print("PAUSE");
	delay(500);

	while (true)	{
		if (digitalRead(JOY_SEL) == 0)	{
			tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
			tft.setCursor(115,50);
			tft.print("PAUSE");
			delay(100);
			tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
			tft.setCursor(135,50);
			tft.print("GO!");
			delay(500);
			tft.setCursor(135,50);
			tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
			tft.print("GO!");
			tft.setRotation(4);
			break;
		}
	}
}

// processJoystick() reads from the joystick to send movement to the cursors position
void processJoystick() {

	int xVal = analogRead(JOY_HORIZ);

	// pauses the game if the joystick is pressed
	if (digitalRead(JOY_SEL) == 0)	{
		pause();
	}

	// moves the cursor's postion based on the joysitck movement and assigns direction for the ships tilt
	// the cursor's movement speed will increase as the joystick is moved further in a given direction
	if (xVal > JOY_CENTER + JOY_DEADZONE && xVal < 900) {
		cursorX += 5;
		ShipDir = 1;
	}
	else if (xVal > 900) {
		cursorX += 10;
		ShipDir = 1;
	}
	else if (xVal < JOY_CENTER - JOY_DEADZONE && xVal > 94) {
		cursorX -= 5;
		ShipDir = -1;
	}
	else if (xVal < 94) {
		cursorX -= 10;
		ShipDir = -1;
	}
	if (cursorX == cursorXOld)	{
		ShipDir = 0;
	}

	// constrains the cursor to within the display
	cursorX = constrain(cursorX, -160, 160);
	// if the cursor reaches a side of the display, relocates it to the opposite side to simulate a 'wrap-around' effect
	if (cursorX == 160) {
		cursorX = -160;
	}
	else if (cursorX == -160) {
		cursorX = 160;
	}

	drawCursor();
	ShipDirOld = ShipDir;
	cursorXOld = cursorX;
	mainGameplay();
}

// collision() tracks the cursors position as well as oncoming objects to check for any overlap between the values which indicates a collision
bool collision() {

	// tracks the tip of the cursor's position
	float tipY = 160 + cursorX;

	// x value of each object's bottom line
	float objectBottom1 = 120 + objectX1 + objectX1/2;
	float objectBottom2 = 120 + objectX2 + objectX2/2;
	float objectBottom3 = 120 + objectX3 + objectX3/2;
	float objectBottom4 = 120 + objectX4 + objectX4/2;
	float objectBottom5 = 120 + objectX5 + objectX5/2;
	float objectBottom6 = 120 + objectX6 + objectX6/2;
	float objectBottom7 = 120 + objectX7 + objectX7/2;
	float objectBottomRange1, objectBottomRange2, objectBottomRange3, objectBottomRange4, objectBottomRange5, objectBottomRange6, objectBottomRange7;
	float yPosNew1, yPosNew2, yPosNew3, yPosNew4, yPosNew5, yPosNew6, yPosNew7;

	// if the bottom line of the object reaches 210...
	if (objectBottom1 >= 210) {
		//... calculate the y-positon of the objects bottom-right corner (different cases are for different "crawling" cases)
		if (yPos1 > 150 && yPos1 < 170) {
			yPosNew1 = yPos1 - 15;
		}
		else if (yPos1 >= 170) {
			yPosNew1 = yPos1 + 30;
		}
		else {
			yPosNew1 = yPos1 - 60;
		}

		// ucalculates the y-position of the object's bottom-left corner
		objectBottomRange1 = yPosNew1 + 30;

		// returns true if the tip cursor is within the two bottom corner's of the object, false if not
		if (tipY >= yPosNew1 && tipY <= objectBottomRange1) {
			return true;
		} else {
			return false;
		}
	}

	// the same procedure is computed for the rest of the objects...

	else if (objectBottom2 >= 210) {
		if (yPos2 > 150 && yPos2 < 170) {
			yPosNew2 = yPos2 - 15;
		}
		else if (yPos2 >= 170) {
			yPosNew2 = yPos2 + 30;
		}
		else {
			yPosNew2 = yPos2 - 60;
		}
		objectBottomRange2 = yPosNew2 + 30;
		if (tipY >= yPosNew2 && tipY <= objectBottomRange2) {
			return true;
		} else {
			return false;
		}
	}

	else if (objectBottom3 >= 210) {
		if (yPos3 > 150 && yPos3 < 170) {
			yPosNew3 = yPos3 - 15;
		}
		else if (yPos3 >= 170) {
			yPosNew3 = yPos3 + 30;
		}
		else {
			yPosNew3 = yPos3 - 60;
		}
		objectBottomRange3 = yPosNew3 + 30;
		if (tipY >= yPosNew3 && tipY <= objectBottomRange3) {
			return true;
		} else {
			return false;
		}
	}

	else if (objectBottom4 >= 210) {
		if (yPos4 > 150 && yPos4 < 170) {
			yPosNew4 = yPos4 - 15;
		}
		else if (yPos4 >= 170) {
			yPosNew4 = yPos4 + 30;
		}
		else {
			yPosNew4 = yPos4 - 60;
		}
		objectBottomRange4 = yPosNew4 + 30;
		if (tipY >= yPosNew4 && tipY <= objectBottomRange4) {
			return true;
		} else {
			return false;
		}
	}

	else if (objectBottom5 >= 210) {
		if (yPos5 > 150 && yPos5 < 170) {
			yPosNew5 = yPos5 - 15;
		}
		else if (yPos5 >= 170) {
			yPosNew5 = yPos5 + 30;
		}
		else {
			yPosNew5 = yPos5 - 60;
		}
		objectBottomRange5 = yPosNew5 + 30;
		if (tipY >= yPosNew5 && tipY <= objectBottomRange5) {
			return true;
		} else {
			return false;
		}
	}

	else if (objectBottom6 >= 210) {
		if (yPos6 > 150 && yPos6 < 170) {
			yPosNew6 = yPos6 - 15;
		}
		else if (yPos6 >= 170) {
			yPosNew6 = yPos6 + 30;
		}
		else {
			yPosNew6 = yPos6 - 60;
		}
		objectBottomRange6 = yPosNew6 + 30;
		if (tipY >= yPosNew6 && tipY <= objectBottomRange6) {
			return true;
		} else {
			return false;
		}
	}

	else if (objectBottom7 >= 210) {
		if (yPos7 > 150 && yPos7 < 170) {
			yPosNew7 = yPos7 - 15;
		}
		else if (yPos7 >= 170) {
			yPosNew7 = yPos7 + 30;
		}
		else {
			yPosNew7 = yPos7 - 60;
		}
		objectBottomRange7 = yPosNew7 + 30;
		if (tipY >= yPosNew7 && tipY <= objectBottomRange7) {
			return true;
		} else {
			return false;
		}
	}

	else {
		return false;
	}
}

// saveScore() saves a score and name to the SD card
void saveScore(int points, String name) {
	file = SD.open("SCORES.TXT", FILE_WRITE);
	Serial.print("Saving score...");
	points = constrain(points, 1, 999);

	// if the score is less than 100 and greater than 10 save a '0' in front, if less than 10 save a '00' in fronnt
	if (file) {
		if (points >= 10 && points < 100) {
			file.print("0");
		} else if (points < 10) {
			file.print("00");
		}

		// saves the user's score and name followed by a new line
		file.print(points);
		file.println(name);
		Serial.println("Score save succsesful!");
		file.close();
	} else {
		Serial.println("Score save unsuccsesful.");
		file.close();
		return;
	}
}

void menu();
void leaderboard();
void play();

// game() displays the menu screen until "Play" or "Leaderboard" is selected
void game() {

	// resets global variables to be used and updated within the actual game
	points = 0;
	levelind = 0;
	beatind = 0;
	i = 0;
	color = ILI9341_WHITE;
	menu();

	// will display the menu until the user chooses an option
	while(true)	{

		if (analogRead(JOY_VERT) < JOY_CENTER - JOY_DEADZONE) {
			highlightedString = constrain(highlightedString + 1, 1, 2);
			menu();
		}
		else if (analogRead(JOY_VERT) > JOY_CENTER + JOY_DEADZONE) {
			highlightedString = constrain(highlightedString - 1, 1, 2);
			menu();
		}

		// if the 'Play' option is selected, a count down is displayed before the game is introduced
		if (digitalRead(JOY_SEL) == 0 && highlightedString == 2)	{
			tft.setRotation(3);
			tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
			tft.setTextSize(6);
			tft.setCursor(140,100);
			tft.fillScreen(ILI9341_BLACK);
			tft.print("3");
			delay(500);
			tft.fillScreen(ILI9341_BLACK);
			tft.setCursor(140,100);
			tft.print("2");
			delay(500);
			tft.fillScreen(ILI9341_BLACK);
			tft.setCursor(140,100);
			tft.print("1");
			delay(500);
			tft.fillScreen(ILI9341_BLACK);
			tft.setCursor(115,100);
			tft.print("GO!");
			delay(500);
			tft.fillScreen(ILI9341_BLACK);
			tft.setRotation(4);

			play();
			break;
		}

		// if the 'Leaderboard' option is selected, the program will go to the leaderboard screen
		else if (digitalRead(JOY_SEL) == 0 && highlightedString == 1) {
			leaderboard();
			break;
		}
	}
}

// menu() displays a menu screen to the display with the options "< Play >" and "< Leaderboard >"
void menu()	{

	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.setTextWrap(false);
	tft.setRotation(3);
	tft.setTextSize(3);
	tft.setCursor(64,50);
	tft.print("CUBE RUNNER");

	if (highlightedString == 2) {
		tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
	}
	else if (highlightedString == 1){
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	}
	tft.setTextSize(1);
	tft.setCursor(135,90);
	tft.print("< Play >");
	tft.setTextSize(1);
	tft.setCursor(117,105);
	if (highlightedString == 1) {
		tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
	}
	else if (highlightedString == 2){
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	}
	tft.print("< Leaderboard >");
	tft.setRotation(4);
	drawBackground();
}

// leaderboard() retrieves the top 5 scores from the already sorted list obtain from the SD card and displays them
// in order a decreasing order (highest to lowest)
void leaderboard() {

	tft.fillScreen(ILI9341_BLACK);
	tft.setRotation(3);
	tft.setTextSize(2);
	tft.setCursor(45,35);
	tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);

	// opens the file from the SD card
	file = SD.open("SCORES.TXT", FILE_READ);
	getHighScores();
	file.close();

	int place = 1;
	tft.print("     SCORE     NAME");
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

	// prints the last 5 scores and names in orderly fashion
	for (int i = 9; i >= 5; i--) {
		tft.setCursor(45,80 + 25*(9-i));
		tft.print(place);
		place += 1;
		// displays dashed lines if there is not enough scores on the SD card
		if (data[i].score == 0 || data[i].name == "") {
			tft.print("     ");
			tft.print("---");
			tft.print("       ");
			tft.print("--");
		} else {
			tft.print("     ");
			if (data[i].score >= 10 && data[i].score < 100) {
				tft.print(" ");
			} else if (data[i].score < 10) {
				tft.print("  ");
			}
			tft.print(data[i].score);
			tft.print("      ");
			tft.print(data[i].name);
		}
	}

	// will not continue until the joystick is pressed
	while (digitalRead(JOY_SEL) != 0) { }

	// reset values stored onto the arduino
	for (int i = 0; i < 10; i++) {
		data[i].score = 0;
		data[i].name = '\0';
	}

	// goes back to the game() function where the original menu is displayed
	tft.setRotation(4);
	tft.fillScreen(ILI9341_BLACK);
	game();
}

// gameOver() displays a Game Over screen as well as the current players score, should the player obtain
// a score that is greater than the 5th score on the leaderboard, the player is given a prompt within the
// serial monitor to enter up to 4 letters representing an alias or name to save to the leaderboard
void gameOver() {

	drawBackground();
	tft.setTextWrap(false);
	tft.setRotation(3);
	tft.setTextSize(3);
	tft.setCursor(80,50);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.print("GAME OVER");
	tft.setTextSize(1);
	tft.setCursor(128,80);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.print("Score: "); tft.print(points);

	char byteRead[5];

	// retrieves the current scores saved onto the SD card for comparison
	file = SD.open("SCORES.TXT", FILE_READ);
	getHighScores();
	file.close();

	// displays special text if the player's score has made it onto the Leaderboard
	if (points > data[5].score) {
		tft.setCursor(95,90);
		tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
		tft.print("New Leaderboard Score!");

		// asks for the players name on the serial monitor to be stored on the SD card
		Serial.print("ENTER YOUR NAME: ");
		for (int i = 0; i < 5; i++) {
			while (Serial.available() == 0) { }

			// will print and save only capital letters, therfore only letters should be used when saving a name
			byteRead[i] = Serial.read() - 32;
			Serial.print(byteRead[i]);

			if (i == 3) {
				byteRead[4] = '\0';
				break;
			}
		}

		Serial.println();
		saveScore(points, byteRead);
	}

	// resets the scores loaded onto the arduino
	for (int i = 0; i < 10; i++) {
		data[i].score = 0;
		data[i].name = '\0';
	}

	delay(1000);

	tft.setRotation(4);
	tft.fillScreen(ILI9341_BLACK);
}

// play() initializes most gameplay/screen features
void play() {

	// resets positional values of each object prior to game start
	objectX1 = 0; objectXOld1 = objectX1; yPos1 = 160;
	objectX2 = 0; objectXOld2 = objectX2; yPos2 = 230;
	objectX3 = 0; objectXOld3 = objectX3; yPos3 = 90;
	objectX4 = 0; objectXOld4 = objectX4; yPos4 = 140;
	objectX5 = 0; objectXOld5 = objectX5; yPos5 = 180;
	objectX6 = 0; objectXOld6 = objectX6; yPos6 = 160;
	objectX7 = 0; objectXOld7 = objectX7; yPos7 = 230;
	SendObject2 = 0; SendObject3 = 0; SendObject4 = 0; SendObject5 = 0; SendObject6 = 0; SendObject7 = 0;

	cursorX = 0;

	// retrieves current scores from SD card for tobeat() function
	file = SD.open("SCORES.TXT", FILE_READ);
	getHighScores();
	file.close();

	toBeat();

	// will display the score to beat as long as there is no collisions
	while (collision() == false)	{

		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
		tft.setTextWrap(false);
		tft.setRotation(3);

		tft.setTextSize(1);
		tft.setCursor(150,5);
		tft.print(points);

		// will change the displayed value to beat everytime the user obtains a score greater than the currently displayed value
		if (points >= data[5+i].score && beatind == 0) {
			beatind = 1;
		}
		if (points < data[9].score && beatind == 1) {
			toBeat();
		}
		// should the player beat the highest possible score on the leaderboard, displaye a "NEW LEADER" text
		if (points >= data[9].score && beatind == 1) {
			tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
			tft.setCursor(260,5);
			tft.print("BEAT: ");
			tft.print(data[5+i].score);
			tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
			tft.setCursor(255, 5);
			tft.print("NEW LEADER!");
			beatind = 2;
		}

		tft.setRotation(4);
		processJoystick();
	}

	// once finished with the score values, reset the values
	for (int i = 0; i < 10; i++) {
		data[i].score = 0;
		data[i].name = '\0';
	}

	tft.setRotation(4);
	tft.fillScreen(ILI9341_BLACK);
	// collision had occured so the gameover screen is displayed
	gameOver();
}

int main() {
	setup();

	// will indefinitely run the game
	while(true) {
		game();
	}

	Serial.end();
	return 0;
}
