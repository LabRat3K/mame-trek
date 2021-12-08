/*
  UI interface for Mame Emulated "Star Trek" arcade game.

  100% Free code to take, re-use, or even claim you wrote it. 
  (It's not good enough to boast over)

  Note: Intended to run on an ATMEGA32U4 based platform, so that 
  the arduino can emulate HID keys and mouse movemnts.

  For a 3d printed arcade encoder: https://www.thingiverse.com/thing:4668322  

*/
#include <Mouse.h>
#include <Keyboard.h>

// Pins for read the clock/data of the rotary encoder
int spinClock = 3;
int spinData = 2;

// Pins and Keyboard key mapping
// List: buttonWarp, buttonPhoton, buttonPhaser, buttonThrust, buttonCoin1, buttonCoin2, buttonPlayer1, buttonPlayer2
char buttonList[8] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, ' ', '5', '6','1','2'};

// Pins to read for each button
char buttonMap[8] = { 4,5,6,7,8,9,10,16};

// Keeping a state history of the button for debouncing
char buttonState[8];

// Global State variables - tracking CLOCK pulses (now and previous reading)
unsigned char gClock = 0;
unsigned char gClockPrev =0;

unsigned char gData = 0;

// Timers to ensure that we don't send mouse data too quickly
unsigned long currentTime;
unsigned long timeout;


// the setup function runs once when you press reset or power the board
void setup() {
	char i ;

	pinMode(spinClock, INPUT);
	pinMode(spinData, INPUT);
	currentTime = millis();
	timeout = currentTime + 5;
	Keyboard.begin();
	Mouse.begin();

	for (i=0;i<sizeof(buttonMap);i++) {
		pinMode(buttonMap[i], INPUT);
		digitalWrite(buttonMap[i], HIGH);
		buttonState[i] = digitalRead(buttonMap[i]);
	}
}

char MouseCount =0 ;
// the loop function runs over and over again forever
void loop() {
  uint8_t i;

	currentTime = millis();

	// --- Read the Spinner CLOCK and DATA pins to determine motion
	//     - output is throttled to prevent too much data 
	// 
	gClock = digitalRead(spinClock);
	gData = digitalRead(spinData);

	if ((!gClock) && (gClockPrev)) {
		if (gData) {
			MouseCount-=5;
		} else {
			MouseCount+=5;
		}
	}
	gClockPrev = gClock;
      
	if (currentTime > timeout) {
		timeout = currentTime + 8;
		// Only send if it has changed 
		if (MouseCount!=0) {
			Mouse.move(MouseCount,0,0);
			MouseCount = 0;
		}
	}

	// --- Scan for buttons being pressed 
	//
	for (i=0;i<sizeof(buttonMap);i++) {
		if (digitalRead(buttonMap[i]) != buttonState[i]) {
			if (buttonState[i] == 1) { 
				Keyboard.press(buttonList[i]);
			} else {
				Keyboard.release(buttonList[i]);
			}
			buttonState[i] = !buttonState[i];
		}
	}
}
