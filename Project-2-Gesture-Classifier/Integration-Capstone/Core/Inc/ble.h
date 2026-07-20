#ifndef BLE_H
#define BLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "classifier.h"
#include "emg.h"
#include "imu.h"
#include <stdint.h>

void BLE_Init(UART_HandleTypeDef *huart);
void BLE_SendTelemetry(Gesture gesture, EMGFeatures *emg, IMUData *imu);

#ifdef __cplusplus
}
#endif

#endif // BLE_H
