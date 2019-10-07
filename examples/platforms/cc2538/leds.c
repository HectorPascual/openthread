#include "gpio.h"
#include "leds.h"

void otPlatformLedsInit(void)
{
    cc2538GpioSoftwareControl(GPIO_D_NUM, LED0_PIN);
    cc2538GpioSoftwareControl(GPIO_D_NUM, LED1_PIN);
    cc2538GpioSoftwareControl(GPIO_D_NUM, LED2_PIN);

    cc2538GpioDirOutput(GPIO_D_NUM, LED0_PIN);
    cc2538GpioDirOutput(GPIO_D_NUM, LED1_PIN);
    cc2538GpioDirOutput(GPIO_D_NUM, LED2_PIN);

    cc2538GpioIocOver(GPIO_D_NUM, LED0_PIN, IOC_OVERRIDE_DIS);
    cc2538GpioIocOver(GPIO_D_NUM, LED1_PIN, IOC_OVERRIDE_DIS);
    cc2538GpioIocOver(GPIO_D_NUM, LED2_PIN, IOC_OVERRIDE_DIS);

    LED_RAINBOW();
}
