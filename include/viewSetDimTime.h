#ifndef VIEW_SETOFFTIME_H
#define VIEW_SETOFFTIME_H

#include "model.h"

class ViewSetDimTime {
public:

static uint8_t selectedIndex;

static int8_t hour;
static int8_t minute;

static void init() {
	selectedIndex = 0;
	hour = DIM_HOUR;
	minute = DIM_MINUTE;
}

static void fixValues() {
	if (hour < 0) {
		hour = 23;
	}

	if (hour > 23) {
		hour = 0;
	}

	if (minute < 0) {
		minute = 59;
	}

	if (minute > 59) {
		minute = 0;
	}
}

static void loop() {
	display.println("Set dim");
	display.println("time");

	printWithLeading(hour);
	display.print(":");
	printWithLeading(minute);
	display.println();

	uint8_t spacing = selectedIndex * 3;

	for (uint8_t i = 0; i < spacing; i++) {
		display.print(" ");
	}

	display.print("^^");
	display.println();

	if (leftPressed()) {
		switch (selectedIndex) {
		case 0:
			hour--;
			break;
		case 1:
			minute--;
			break;
		}
	}

	if (selectPressed()) {
		selectedIndex++;
	}

	if (rightPressed()) {
		switch (selectedIndex) {
		case 0:
			hour++;
			break;
		case 1:
			minute++;
			break;
		}
	}

	fixValues();

	if (selectedIndex > 1) {
		DIM_HOUR = hour;
		DIM_MINUTE = minute;
		EEPROM.write(0, hour);
		EEPROM.write(1, minute);

		VIEW = 0;
	}
}
};

uint8_t ViewSetDimTime::selectedIndex;

int8_t ViewSetDimTime::hour;
int8_t ViewSetDimTime::minute;

#endif
