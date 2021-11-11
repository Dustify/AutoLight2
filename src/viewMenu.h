#ifndef VIEW_MENU_H
#define VIEW_MENU_H

#include "model.h"
#include "viewSetTime.h"
#include "viewSetDate.h"
#include "viewSetOffTime.h"

class ViewMenu {
public:
static uint8_t viewIndex;
static int8_t selectedIndex;

static void fixSelected() {
	const uint8_t menuCount = 3;

	if (selectedIndex < viewIndex || selectedIndex > (viewIndex + 2)) {
		viewIndex = selectedIndex;
	}

	if (selectedIndex > menuCount) {
		selectedIndex = 0;
	}

	if (selectedIndex < 0) {
		selectedIndex = menuCount;
	}
}

static void markSelected(uint8_t index) {
	if (selectedIndex == index) {
		display.print(">");
		return;
	}

	display.print(" ");
}

static void printMenuOption(uint8_t index, const char *text) {
	if (index < viewIndex || index > (viewIndex + 2)) {
		return;
	}

	markSelected(index);
	display.print(text);
	display.println();
}

static void loop() {
	display.print("Menu");
	display.println();

	printMenuOption(0, "Time");
	printMenuOption(1, "Date");
	printMenuOption(2, "Off Time");
	printMenuOption(3, "Exit");

	if (leftPressed()) {
		selectedIndex--;
	}

	if (selectPressed()) {
		switch (selectedIndex) {
		case 0:
			ViewSetTime::init();
			VIEW = 2;
			break;
		case 1:
			ViewSetDate::init();
			VIEW = 3;
			break;
		case 2:
			ViewSetOffTime::init();
			VIEW = 4;
			break;
		case 3:
			VIEW = 0;
			break;
		}

		selectedIndex = 0;
	}

	if (rightPressed()) {
		selectedIndex++;
	}

	fixSelected();
}
};

uint8_t ViewMenu::viewIndex;
int8_t ViewMenu::selectedIndex;

#endif
