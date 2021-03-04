#include <AFG_Led.h>

AFG_Led::AFG_Led(int pin) {
	this->pin_led = pin;
	this->definedBoard = BoardType::ArduinoBoard;
	pinMode(this->pin_led, OUTPUT);
	ledON = HIGH;
	ledOff = LOW;
}

AFG_Led::AFG_Led(int pin, BoardType board) {
	this->pin_led = pin;
	this->definedBoard = board;
	pinMode(this->pin_led, OUTPUT);
	if (this->definedBoard == BoardType::ArduinoBoard) {
		ledON = HIGH;
		ledOff = LOW;
	} else {
		ledON = LOW;
		ledOff = HIGH;
	}
}

void AFG_Led::turnOn() {
	digitalWrite(this->pin_led, ledON);
}

void AFG_Led::turnOff() {
	digitalWrite(this->pin_led, ledOff);
}

void AFG_Led::turnOn_Wait(int milliseconds) {
	digitalWrite(this->pin_led, ledON);
	delay(milliseconds);
	digitalWrite(this->pin_led, ledOff);
}

void AFG_Led::turnOff_Wait(int milliseconds) {
	digitalWrite(this->pin_led, ledOff);
	delay(milliseconds);
	digitalWrite(this->pin_led, ledON);
}

void AFG_Led::blink(int blinks) {
	for (int i = 0; i < blinks; i++) {
		turnOn_Wait(1000);
		delay(1000);
	}
}

void AFG_Led::blink(int blinks, int milliseconds) {
	for (int i = 0; i < blinks; i++) {
		turnOn_Wait(milliseconds);
		delay(milliseconds);
	}
}