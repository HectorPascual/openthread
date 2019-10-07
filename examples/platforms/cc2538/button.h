#ifndef BUTTON_H
#define BUTTON_H

#include "gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define BUTTON_USER_PORT GPIO_A_NUM
#define BUTTON_USER_PIN 3

void init_button(void);
int read_button(void);

#ifdef __cplusplus
} // end extern "C"
#endif
#endif // BUTTON_H
