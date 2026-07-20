#ifndef SERVO_H
#define SERVO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "classifier.h"
#include <stdint.h>

#define PCA9685_ADDR    0x40
#define PCA9685_MODE1   0x00
#define PCA9685_PRESCALE 0xFE

void Servo_Init(I2C_HandleTypeDef *hi2c);
void Servo_SetGesture(Gesture gesture);

#ifdef __cplusplus
}
#endif

#endif // SERVO_H
