#ifndef EMG_H
#define EMG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>

// ADS1115 I2C address (ADDR pin tied to GND)
#define ADS1115_ADDR        0x48

// ADS1115 register addresses
#define ADS1115_REG_CONVERT 0x00
#define ADS1115_REG_CONFIG  0x01

// Config register — single shot, AIN0 vs GND, ±4.096V, 860 SPS
#define ADS1115_CFG_CH0     0xC3E3
// Config register — single shot, AIN1 vs GND, ±4.096V, 860 SPS
#define ADS1115_CFG_CH1     0xD3E3

// Number of samples per feature window
#define EMG_WINDOW_SIZE     200

typedef struct {
    float rms_ch1;
    float rms_ch2;
    float mav_ch1;   // Mean Absolute Value
    float mav_ch2;
} EMGFeatures;

// Initialize EMG module
void EMG_Init(I2C_HandleTypeDef *hi2c);

// Read one sample from ADS1115 channel (0 or 1)
int16_t EMG_ReadChannel(uint8_t channel);

// Fill a window of samples for one channel
void EMG_CollectWindow(uint8_t channel, int16_t *buffer, uint16_t size);

// Compute features from a sample window
void EMG_ComputeFeatures(int16_t *ch1_buf, int16_t *ch2_buf,
                          uint16_t size, EMGFeatures *features);

#ifdef __cplusplus
}
#endif

#endif // EMG_H
