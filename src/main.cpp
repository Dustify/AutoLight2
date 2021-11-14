#include "Adafruit_I2CDevice.h"

#include "model.h"
#include "viewHome.h"
#include "viewMenu.h"
#include "viewSetTime.h"
#include "viewSetOffTime.h"

// Hello, dad!

void setup()
{
	modelInit();
}

void loop()
{
	modelUpdate();

	display.clearDisplay();
	display.setCursor(0, 0);

	switch (VIEW)
	{
	case 0:
		ViewHome::loop();
		break;
	case 1:
		ViewMenu::loop();
		break;
	case 2:
		ViewSetTime::loop();
		break;
	case 3:
		ViewSetDate::loop();
		break;
	case 4:
		ViewSetOffTime::loop();
		break;
	}

	display.display();
}
