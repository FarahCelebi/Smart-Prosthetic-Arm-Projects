#include "imu.h"

static SPI_HandleTypeDef *_hspi = nullptr;

static void CS_Low()  {
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_RESET);
}
static void CS_High() {
    HAL_GPIO_WritePin(IMU_CS_PORT, IMU_CS_PIN, GPIO_PIN_SET);
}

static void IMU_WriteReg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = {(uint8_t)(reg & 0x7F), val};
    CS_Low();
    HAL_SPI_Transmit(_hspi, tx, 2, 10);
    CS_High();
}

void IMU_Init(SPI_HandleTypeDef *hspi) {
    _hspi = hspi;
    CS_High();
    HAL_Delay(10);
    IMU_WriteReg(ICM_REG_PWR_MGMT1, 0x01);
    HAL_Delay(10);
}

void IMU_Read(IMUData *data) {
    uint8_t buf[12];
    uint8_t tx = ICM_REG_ACCEL_XOUT | 0x80;
    CS_Low();
    HAL_SPI_Transmit(_hspi, &tx, 1, 10);
    HAL_SPI_Receive(_hspi, buf, 12, 50);
    CS_High();

    int16_t ax = (int16_t)((buf[0]  << 8) | buf[1]);
    int16_t ay = (int16_t)((buf[2]  << 8) | buf[3]);
    int16_t az = (int16_t)((buf[4]  << 8) | buf[5]);
    int16_t gx = (int16_t)((buf[6]  << 8) | buf[7]);
    int16_t gy = (int16_t)((buf[8]  << 8) | buf[9]);
    int16_t gz = (int16_t)((buf[10] << 8) | buf[11]);

    data->accel_x = ax / 16384.0f;
    data->accel_y = ay / 16384.0f;
    data->accel_z = az / 16384.0f;
    data->gyro_x  = gx / 131.0f;
    data->gyro_y  = gy / 131.0f;
    data->gyro_z  = gz / 131.0f;
}
