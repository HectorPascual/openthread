#include "gpio.h"
#include "button.h"

void init_button(){
    cc2538GpioHardwareControl(BUTTON_USER_PORT, BUTTON_USER_PIN);

    cc2538GpioDirInput(BUTTON_USER_PORT, BUTTON_USER_PIN);

    cc2538GpioIocOver(BUTTON_USER_PORT, BUTTON_USER_PIN, IOC_OVERRIDE_DIS);

}

int read_button(){
	if (cc2538GpioReadPin(BUTTON_USER_PORT, BUTTON_USER_PIN)){
		return 1;
	}
	else {
		return 0;
	}
}