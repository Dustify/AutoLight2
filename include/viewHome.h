#ifndef VIEW_HOME_H
#define VIEW_HOME_H

#include "model.h"

class ViewHome
{
private:
	static bool timeoutNeedsReset;
	static unsigned long timeoutStart;

public:
	static void loop()
	{
		unsigned long now = millis();

		if (timeoutNeedsReset)
		{
			timeoutNeedsReset = false;
			timeoutStart = now;
		}

		if (leftPressed() || rightPressed())
		{
			MAIN_OVERRIDE = !MAIN_OVERRIDE;
			timeoutStart = now;
		}

		if (dimmerPressed())
		{
			DIM_OVERRIDE = !DIM_OVERRIDE;
			timeoutStart = now;
		}

		bool timedOut = (now - timeoutStart) > HOME_TIMEOUT * 1e3;

		if (timedOut)
		{
			if (!selectPressed())
			{
				display.clearDisplay();
				return;
			}
			else
			{
				timeoutStart = now;
			}
		}

		printWithLeading(HOUR);
		display.print(":");
		printWithLeading(MINUTE);
		display.print(":");
		printWithLeading(SECOND);
		display.println();

		printWithLeading(DAY);
		display.print("/");
		printWithLeading(MONTH);
		display.print("/");
		display.print(YEAR);
		display.println();

		if (MAIN_STATE)
		{
			// if on
			if (!DIM_STATE)
			{
				// if not dimmed
				display.print("On ");
				printWithLeading(DIM_HOUR);
				display.print(":");
				printWithLeading(DIM_MINUTE);
			}
			else
			{
				// if dimmed
				display.print("Dim ");
				printWithLeading(SR_HOUR);
				display.print(":");
				printWithLeading(SR_MINUTE);
			}
		}
		else
		{
			// if off
			display.print("Off ");
			printWithLeading(SS_HOUR);
			display.print(":");
			printWithLeading(SS_MINUTE);
		}

		display.println();

		if (MAIN_OVERRIDE || DIM_OVERRIDE)
		{
			if (MAIN_OVERRIDE)
			{
				display.print("MOR ");
			}

			if (DIM_OVERRIDE)
			{
				display.print("DOR");
			}
		}
		else
		{
			if (PIR_TIMER > 0)
			{
				display.print(PIR_TIMER / (float)1000);
			}
			else
			{
				display.print(TEMPERATURE);
				display.print((char)248);
				display.print("C");
			}
		}

		display.println();

		if (selectPressed())
		{
			timeoutNeedsReset = true;
			VIEW = 1;
		}
	}
};

bool ViewHome::timeoutNeedsReset;
unsigned long ViewHome::timeoutStart;

#endif
