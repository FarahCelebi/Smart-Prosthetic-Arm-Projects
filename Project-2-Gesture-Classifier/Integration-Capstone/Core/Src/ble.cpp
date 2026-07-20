#include "ble.h"
#include <stdio.h>
#include <string.h>

static UART_HandleTypeDef *_huart = nullptr;

void BLE_Init(UART_HandleTypeDef *huart) {
    _huart = huart;
}

void BLE_SendTelemetry(Gesture gesture, EMGFeatures *emg, IMUData *imu) {
    char buf[128];
    int ax = (int)(imu->accel_x * 100);
    int ay = (int)(imu->accel_y * 100);
    int az = (int)(imu->accel_z * 100);
    int gz = (int)(imu->gyro_z * 10);
    int e1 = (int)emg->rms_ch1;
    int e2 = (int)emg->rms_ch2;

    int len = snprintf(buf, sizeof(buf),
        "G:%d,E1:%d,E2:%d,AX:%d,AY:%d,AZ:%d,GZ:%d\r\n",
        (int)gesture, e1, e2, ax, ay, az, gz);
    HAL_UART_Transmit(_huart, (uint8_t *)buf, (uint16_t)len, 100);
}
