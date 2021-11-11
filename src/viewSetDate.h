#ifndef VIEW_SETDATE_H
#define VIEW_SETDATE_H

#include "model.h"

class ViewSetDate {
public:

static uint8_t selectedIndex;

static int8_t day;
static int8_t month;
static int16_t year;

static void init() {
	selectedIndex = 0;
	day = DAY;
	month = MONTH;
	year = YEAR;
}

static void fixValues() {
	if (day < 0) {
		day = 31;
	}

	if (day > 31) {
		day = 0;
	}

	if (month < 0) {
		month = 12;
	}

	if (month > 12) {
		month = 0;
	}

	if (year < 2000) {
	 year = 2231;
	}

	if (year > 2231) {
	 year = 2000;
	}
}

static void loop() {
	display.print("Set date");
	display.println();

	printWithLeading(day);
	display.print("/");
	printWithLeading(month);
	display.print("/");
	printWithLeading(year);
	display.println();

	uint8_t spacing = selectedIndex * 3;

	for (uint8_t i = 0; i < spacing; i++) {
		display.print(" ");
	}

	display.print("^^");

	if (selectedIndex == 2) {
		display.print("^^");
	}

	display.println();

	if (leftPressed()) {
		switch (selectedIndex) {
		case 0:
			day--;
			break;
		case 1:
			month--;
			break;
		case 2:
			year--;
			break;
		}
	}

	if (selectPressed()) {
		selectedIndex++;
	}

	if (rightPressed()) {
		switch (selectedIndex) {
		case 0:
			day++;
			break;
		case 1:
			month++;
			break;
		case 2:
			year++;
			break;
		}
	}

	fixValues();

	if (selectedIndex > 2) {
		RtcDateTime oldValue = rtc.GetDateTime();
		RtcDateTime newValue =
			RtcDateTime(
				year,
				month,
				day,
				oldValue.Hour(),
				oldValue.Minute(),
				oldValue.Second());

		rtc.SetDateTime(newValue);
		VIEW = 0;
	}
}
};

uint8_t ViewSetDate::selectedIndex;

int8_t ViewSetDate::day;
int8_t ViewSetDate::month;
int16_t ViewSetDate::year;

#endif
