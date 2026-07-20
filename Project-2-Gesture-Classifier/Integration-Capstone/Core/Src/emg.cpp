#include "emg.h"
#include <math.h>

// Private handle pointer
static I2C_HandleTypeDef *_hi2c = nullptr;

void EMG_Init(I2C_HandleTypeDef *hi2c) {
    _hi2c = hi2c;
}

int16_t EMG_ReadChannel(uint8_t channel) {
    uint8_t config[3];
    uint16_t cfg_value = (channel == 0) ? ADS1115_CFG_CH0 : ADS1115_CFG_CH1;

    // Write config register to start single conversion
    config[0] = ADS1115_REG_CONFIG;
    config[1] = (cfg_value >> 8) & 0xFF;
    config[2] = cfg_value & 0xFF;
    HAL_I2C_Master_Transmit(_hi2c, ADS1115_ADDR << 1, config, 3, 100);

    // Wait for conversion — at 860 SPS, ~1.2ms per sample
    HAL_Delay(2);

    // Point to conversion register
    uint8_t reg = ADS1115_REG_CONVERT;
    HAL_I2C_Master_Transmit(_hi2c, ADS1115_ADDR << 1, &reg, 1, 100);

    // Read 2 bytes of result
    uint8_t data[2];
    HAL_I2C_Master_Receive(_hi2c, ADS1115_ADDR << 1, data, 2, 100);

    // Combine bytes into signed 16-bit value
    int16_t result = (int16_t)((data[0] << 8) | data[1]);
    return result;
}

void EMG_CollectWindow(uint8_t channel, int16_t *buffer, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        buffer[i] = EMG_ReadChannel(channel);
    }
}

void EMG_ComputeFeatures(int16_t *ch1_buf, int16_t *ch2_buf,
                          uint16_t size, EMGFeatures *features) {
    float sum_sq_ch1 = 0.0f;
    float sum_sq_ch2 = 0.0f;
    float sum_abs_ch1 = 0.0f;
    float sum_abs_ch2 = 0.0f;

    for (uint16_t i = 0; i < size; i++) {
        float s1 = (float)ch1_buf[i];
        float s2 = (float)ch2_buf[i];

        sum_sq_ch1  += s1 * s1;
        sum_sq_ch2  += s2 * s2;
        sum_abs_ch1 += fabsf(s1);
        sum_abs_ch2 += fabsf(s2);
    }

    features->rms_ch1 = sqrtf(sum_sq_ch1 / size);
    features->rms_ch2 = sqrtf(sum_sq_ch2 / size);
    features->mav_ch1 = sum_abs_ch1 / size;
    features->mav_ch2 = sum_abs_ch2 / size;
}