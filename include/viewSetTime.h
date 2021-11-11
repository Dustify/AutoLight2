#ifndef VIEW_SETTIME_H
#define VIEW_SETTIME_H

#include "model.h"

class ViewSetTime {
public:

static uint8_t selectedIndex;

static int8_t hour;
static int8_t minute;
static int8_t second;

static void init() {
	selectedIndex = 0;
	hour = HOUR;
	minute = MINUTE;
	second = SECOND;
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

	if (second < 0) {
		second = 59;
	}

	if (second > 59) {
		second = 0;
	}
}

static void loop() {
	display.print("Set time");
	display.println();

	printWithLeading(hour);
	display.print(":");
	printWithLeading(minute);
	display.print(":");
	printWithLeading(second);
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
		case 2:
			second--;
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
		case 2:
			second++;
			break;
		}
	}

	fixValues();

	if (selectedIndex > 2) {
		RtcDateTime oldValue = rtc.GetDateTime();
		RtcDateTime newValue =
			RtcDateTime(
				oldValue.Year(),
				oldValue.Month(),
				oldValue.Day(),
				hour,
				minute,
				second);

		rtc.SetDateTime(newValue);
		VIEW = 0;
	}
}
};

uint8_t ViewSetTime::selectedIndex;

int8_t ViewSetTime::hour;
int8_t ViewSetTime::minute;
int8_t ViewSetTime::second;

#endif
