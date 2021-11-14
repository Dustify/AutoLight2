#ifndef MODEL_H
#define MODEL_H

#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS3231.h>
#include <TimeLord.h>
#include <EEPROM.h>

Adafruit_SSD1306 display(128, 64);

RtcDS3231<TwoWire> rtc(Wire);

TimeLord timeLord;

uint8_t VIEW;

uint8_t STATE_LEFT;
uint8_t STATE_SELECT;
uint8_t STATE_RIGHT;

uint8_t STATE_LEFT_CHANGED;
uint8_t STATE_SELECT_CHANGED;
uint8_t STATE_RIGHT_CHANGED;

uint8_t HOUR;
uint8_t MINUTE;
uint8_t SECOND;
uint8_t DAY;
uint8_t MONTH;
uint16_t YEAR;
uint8_t YEAR_SHORT;

float TIME;

uint8_t SR_HOUR;
uint8_t SR_MINUTE;
float SUNRISE;

uint8_t SS_HOUR;
uint8_t SS_MINUTE;
float SUNSET;

uint8_t SUNRISE_NEXT = 0;

float TEMPERATURE;

uint8_t OFF_HOUR;
uint8_t OFF_MINUTE;

float OFF_TIME;

bool STATE;
bool OVERRIDE;

void modelInit()
{
	pinMode(PIN_LEFT, INPUT_PULLUP);
	pinMode(PIN_SELECT, INPUT_PULLUP);
	pinMode(PIN_RIGHT, INPUT_PULLUP);

	pinMode(PIN_OUTPUT, OUTPUT);

	display.begin(SSD1306_SWITCHCAPVCC, 0x3c);

	display.setTextColor(WHITE);
	display.setTextSize(2);
	
	rtc.Begin();

	timeLord.TimeZone(TIMEZONE * 60);
	timeLord.Position(LONGITUDE, LATITUDE);

	OFF_HOUR = EEPROM.read(0);
	OFF_MINUTE = EEPROM.read(1);
}

void printWithLeading(uint16_t value)
{
	if (value < 10)
	{
		display.print(0);
	}

	display.print(value);
}

uint8_t leftPressed()
{
	if (STATE_LEFT == LOW && STATE_LEFT_CHANGED)
	{
		STATE_LEFT_CHANGED = 0;
		return 1;
	}

	return 0;
}

uint8_t selectPressed()
{
	if (STATE_SELECT == LOW && STATE_SELECT_CHANGED)
	{
		STATE_SELECT_CHANGED = 0;
		return 1;
	}

	return 0;
}

uint8_t rightPressed()
{
	if (STATE_RIGHT == LOW && STATE_RIGHT_CHANGED)
	{
		STATE_RIGHT_CHANGED = 0;
		return 1;
	}

	return 0;
}

void modelUpdate()
{
	uint8_t STATE_LEFT_NEW = digitalRead(PIN_LEFT);
	uint8_t STATE_SELECT_NEW = digitalRead(PIN_SELECT);
	uint8_t STATE_RIGHT_NEW = digitalRead(PIN_RIGHT);

	if (STATE_LEFT_NEW != STATE_LEFT)
	{
		STATE_LEFT_CHANGED = 1;
	}

	if (STATE_SELECT_NEW != STATE_SELECT)
	{
		STATE_SELECT_CHANGED = 1;
	}

	if (STATE_RIGHT_NEW != STATE_RIGHT)
	{
		STATE_RIGHT_CHANGED = 1;
	}

	STATE_LEFT = STATE_LEFT_NEW;
	STATE_SELECT = STATE_SELECT_NEW;
	STATE_RIGHT = STATE_RIGHT_NEW;

	RtcDateTime now = rtc.GetDateTime();

	HOUR = now.Hour();
	MINUTE = now.Minute();
	SECOND = now.Second();
	DAY = now.Day();
	MONTH = now.Month();
	YEAR = now.Year();
	YEAR_SHORT = YEAR - 2000;

	TIME = HOUR + ((float)MINUTE / 60);

	byte nowArray[] = {SECOND, MINUTE, HOUR, DAY, MONTH, YEAR_SHORT};

	timeLord.SunRise(nowArray);
	SR_MINUTE = nowArray[1];
	SR_HOUR = nowArray[2];

	SUNRISE = SR_HOUR + ((float)SR_MINUTE / 60);

	timeLord.SunSet(nowArray);
	SS_MINUTE = nowArray[1];
	SS_HOUR = nowArray[2];

	SUNSET = SS_HOUR + ((float)SS_MINUTE / 60);

	if (TIME > SUNSET)
	{
		nowArray[3]++;

		timeLord.SunSet(nowArray);
		SS_MINUTE = nowArray[1];
		SS_HOUR = nowArray[2];

		SUNSET = SS_HOUR + ((float)SS_MINUTE / 60);
	}

	SUNRISE_NEXT = TIME < SUNRISE || TIME > SUNSET ? 1 : 0;

	RtcTemperature temperature = rtc.GetTemperature();

	if (TEMP_IN_CELCIUS)
	{
		TEMPERATURE = temperature.AsFloatDegC();
	}
	else
	{
		TEMPERATURE = temperature.AsFloatDegF();
	}

	OFF_TIME = OFF_HOUR + ((float)OFF_MINUTE / 60);

	if (OFF_TIME > SUNSET)
	{
		STATE = TIME >= SUNSET && TIME < OFF_TIME;
	}
	else
	{
		STATE = TIME < SUNSET && TIME < OFF_TIME;
	}

	digitalWrite(PIN_OUTPUT, OVERRIDE || STATE ? HIGH : LOW);
}

#endif
