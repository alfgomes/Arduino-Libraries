#ifndef AFG_LED_H_INCLUDED
#define AFG_LED_H_INCLUDED

#include <Arduino.h>

//enum Motion { UP, DOWN, STOP };
enum BoardType: int { ArduinoBoard, Esp8266Board, Esp01Board };
	
class AFG_Led {
public:
	AFG_Led(int pin);
	AFG_Led(int pin, BoardType board);
	void turnOn();
	void turnOff();
	void turnOn_Wait(int milliseconds);
	void turnOff_Wait(int milliseconds);
	void blink(int blinks);
	void blink(int blinks, int milliseconds);
private:
	BoardType definedBoard;
	int pin_led;
	int ledON = LOW;
	int ledOff = HIGH;
};

#endif