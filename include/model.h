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
uint8_t STATE_DIMMER;

uint8_t STATE_LEFT_CHANGED;
uint8_t STATE_SELECT_CHANGED;
uint8_t STATE_RIGHT_CHANGED;
uint8_t STATE_DIMMER_CHANGED;

uint8_t HOUR;
uint8_t MINUTE;
uint8_t SECOND;
uint8_t DAY;
uint8_t MONTH;
uint16_t YEAR;

uint8_t SR_HOUR;
uint8_t SR_MINUTE;

uint8_t SS_HOUR;
uint8_t SS_MINUTE;

uint8_t SUNRISE_NEXT = 0;

float TEMPERATURE;

uint8_t DIM_HOUR;
uint8_t DIM_MINUTE;

bool MAIN_STATE;
bool MAIN_OVERRIDE;

bool DIM_STATE;
bool DIM_OVERRIDE;

unsigned long LAST_TIMESTAMP = 0;
unsigned long PIR_TIMER = 0;

bool LAST_LOOP_WAS_DUSK_TO_DAWN = false;

void modelInit()
{
	pinMode(PIN_LEFT, INPUT_PULLUP);
	pinMode(PIN_SELECT, INPUT_PULLUP);
	pinMode(PIN_RIGHT, INPUT_PULLUP);
	pinMode(PIN_DIMMER, INPUT_PULLUP);

	pinMode(PIN_PIR, INPUT);

	pinMode(PIN_OUT_MAIN, OUTPUT);
	pinMode(PIN_OUT_DIM, OUTPUT);
	pinMode(PIN_OUT_DIM_INV, OUTPUT);

	display.begin(SSD1306_SWITCHCAPVCC, 0x3c);

	display.setTextColor(WHITE);
	display.setTextSize(2);

	rtc.Begin();

	timeLord.TimeZone(TIMEZONE * 60);
	timeLord.Position(LONGITUDE, LATITUDE);

	DIM_HOUR = EEPROM.read(0);
	DIM_MINUTE = EEPROM.read(1);
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

uint8_t dimmerPressed()
{
	if (STATE_DIMMER == LOW && STATE_DIMMER_CHANGED)
	{
		STATE_DIMMER_CHANGED = 0;
		return 1;
	}

	return 0;
}

void modelUpdate()
{
	unsigned long CURRENT_TIMESTAMP = millis();
	unsigned long DURATION = CURRENT_TIMESTAMP - LAST_TIMESTAMP;
	LAST_TIMESTAMP = CURRENT_TIMESTAMP;

	// begin button press handling
	uint8_t STATE_LEFT_NEW = digitalRead(PIN_LEFT);
	uint8_t STATE_SELECT_NEW = digitalRead(PIN_SELECT);
	uint8_t STATE_RIGHT_NEW = digitalRead(PIN_RIGHT);
	uint8_t STATE_DIMMER_NEW = digitalRead(PIN_DIMMER);

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

	if (STATE_DIMMER_NEW != STATE_DIMMER)
	{
		STATE_DIMMER_CHANGED = 1;
	}

	STATE_LEFT = STATE_LEFT_NEW;
	STATE_SELECT = STATE_SELECT_NEW;
	STATE_RIGHT = STATE_RIGHT_NEW;
	STATE_DIMMER = STATE_DIMMER_NEW;
	// end button press handling

	// begin time retrieval
	RtcDateTime now = rtc.GetDateTime();

	HOUR = now.Hour();
	MINUTE = now.Minute();
	SECOND = now.Second();
	DAY = now.Day();
	MONTH = now.Month();
	YEAR = now.Year();
	uint8_t YEAR_SHORT = YEAR - 2000;

	// calc as float for comparison
	float TIME = HOUR + ((float)MINUTE / 60);
	// end time retrieval

	// begin calc sunrise / sunset

	// store current datetime in array
	byte nowArray[] = {SECOND, MINUTE, HOUR, DAY, MONTH, YEAR_SHORT};

	// find sunrise (will update array in place)
	timeLord.SunRise(nowArray);
	SR_MINUTE = nowArray[1];
	SR_HOUR = nowArray[2];

	// calc as float
	float SUNRISE = SR_HOUR + ((float)SR_MINUTE / 60);

	// find sunset (will update array in place)
	timeLord.SunSet(nowArray);
	SS_MINUTE = nowArray[1];
	SS_HOUR = nowArray[2];

	// calc as float
	float SUNSET = SS_HOUR + ((float)SS_MINUTE / 60);

	// check if already past sunset
	if (TIME > SUNSET)
	{
		// if already past sunset, increment day
		nowArray[3]++;

		// find sunset 'for tomorrow'
		timeLord.SunSet(nowArray);
		SS_MINUTE = nowArray[1];
		SS_HOUR = nowArray[2];

		// update calc as float
		SUNSET = SS_HOUR + ((float)SS_MINUTE / 60);
	}

	// calc if sunrise is next event
	// first part handles midnight to sunrise
	// second part handles sunset to midnight
	SUNRISE_NEXT = TIME < SUNRISE || TIME > SUNSET ? 1 : 0;

	// end calc sunrise / sunset

	// begin rtc temp (remove?)
	RtcTemperature temperature = rtc.GetTemperature();
	TEMPERATURE = temperature.AsFloatDegC();
	// end rtc temp (remove?)

	MAIN_STATE = TIME >= SUNSET || TIME < SUNRISE;

	// calc dim time as float
	float DIM_TIME = DIM_HOUR + ((float)DIM_MINUTE / 60);

	// dim if past dim time or before sunrise
	DIM_STATE = TIME >= DIM_TIME || TIME < SUNRISE;

	bool MAIN_STATE_FINAL = MAIN_STATE;
	bool DIM_STATE_FINAL = DIM_STATE;

	// turn off dimmer ssr if during day

	bool DAWN_TO_DUSK = TIME >= SUNRISE && TIME < SUNSET;

	if (DAWN_TO_DUSK)
	{
		DIM_STATE_FINAL = true;
	}

	/// begin PIR handling
	if (PIR_TIMER > 0)
	{
		if (PIR_TIMER < DURATION)
		{
			PIR_TIMER = 0;
		}
		else
		{
			PIR_TIMER = PIR_TIMER - DURATION;
		}
	}

	uint8_t STATE_PIR = digitalRead(PIN_PIR);

	if (STATE_PIR == LOW)
	{
		PIR_TIMER = PIR_TIMEOUT * 1000;
	}

	MAIN_STATE_FINAL = MAIN_STATE_FINAL && PIR_TIMER > 0;
	/// end PIR handling

	// begin override handling
	if (MAIN_OVERRIDE)
	{
		MAIN_STATE_FINAL = !MAIN_STATE_FINAL;
	}

	if (DIM_OVERRIDE)
	{
		DIM_STATE_FINAL = !DIM_STATE_FINAL;
	}
	//end override handling

	digitalWrite(PIN_OUT_MAIN, MAIN_STATE_FINAL ? HIGH : LOW);
	digitalWrite(PIN_OUT_DIM, DIM_STATE_FINAL ? LOW : HIGH);
	digitalWrite(PIN_OUT_DIM_INV, DIM_STATE_FINAL && (!DAWN_TO_DUSK || MAIN_STATE_FINAL) ? HIGH : LOW);

	if (DAWN_TO_DUSK && LAST_LOOP_WAS_DUSK_TO_DAWN)
	{
		MAIN_OVERRIDE = false;
		DIM_OVERRIDE = false;
	}

	LAST_LOOP_WAS_DUSK_TO_DAWN = !DAWN_TO_DUSK;
}

#endif