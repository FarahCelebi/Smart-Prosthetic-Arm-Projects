#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define IMU_CS_PORT     GPIOA
#define IMU_CS_PIN      GPIO_PIN_4

#define ICM_REG_PWR_MGMT1   0x4E
#define ICM_REG_ACCEL_XOUT  0x1F
#define ICM_REG_GYRO_XOUT   0x25

typedef struct {
    float accel_x, accel_y, accel_z;
    float gyro_x,  gyro_y,  gyro_z;
} IMUData;

void IMU_Init(SPI_HandleTypeDef *hspi);
void IMU_Read(IMUData *data);

#ifdef __cplusplus
}
#endif

#endif // IMU_H
