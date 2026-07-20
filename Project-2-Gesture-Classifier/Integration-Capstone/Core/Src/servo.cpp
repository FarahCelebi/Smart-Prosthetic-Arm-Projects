#include "servo.h"

static I2C_HandleTypeDef *_hi2c = nullptr;

static void PCA_WriteReg(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    HAL_I2C_Master_Transmit(_hi2c, PCA9685_ADDR << 1, buf, 2, 100);
}

static void PCA_SetChannel(uint8_t ch, uint16_t on, uint16_t off) {
    uint8_t buf[5];
    buf[0] = 0x06 + (ch * 4);
    buf[1] = on & 0xFF;
    buf[2] = (on >> 8) & 0xFF;
    buf[3] = off & 0xFF;
    buf[4] = (off >> 8) & 0xFF;
    HAL_I2C_Master_Transmit(_hi2c, PCA9685_ADDR << 1, buf, 5, 100);
}

void Servo_Init(I2C_HandleTypeDef *hi2c) {
    _hi2c = hi2c;
    PCA_WriteReg(PCA9685_MODE1, 0x00);
    HAL_Delay(10);
    // Set prescaler for 50Hz: 25MHz / (4096 * 50) - 1 = 121
    PCA_WriteReg(PCA9685_MODE1, 0x10); // sleep
    PCA_WriteReg(PCA9685_PRESCALE, 121);
    PCA_WriteReg(PCA9685_MODE1, 0x00); // wake
    HAL_Delay(5);
}

// Pulse width: REST=307 (~1.5ms), OPEN=205 (~1ms), CLOSE=410 (~2ms)
void Servo_SetGesture(Gesture gesture) {
    uint16_t pos[6];

    switch (gesture) {
        case GESTURE_REST:
            for (int i = 0; i < 6; i++) pos[i] = 307;
            break;
        case GESTURE_GRIP:
            for (int i = 0; i < 6; i++) pos[i] = 410;
            break;
        case GESTURE_PINCH:
            pos[0] = 410; pos[1] = 410;
            pos[2] = 307; pos[3] = 307;
            pos[4] = 307; pos[5] = 307;
            break;
        case GESTURE_POINT:
            pos[0] = 205; pos[1] = 410;
            pos[2] = 410; pos[3] = 410;
            pos[4] = 410; pos[5] = 307;
            break;
        case GESTURE_PRONATE:
            for (int i = 0; i < 5; i++) pos[i] = 307;
            pos[5] = 205;
            break;
        case GESTURE_SUPINATE:
            for (int i = 0; i < 5; i++) pos[i] = 307;
            pos[5] = 410;
            break;
        default:
            for (int i = 0; i < 6; i++) pos[i] = 307;
            break;
    }

    for (uint8_t i = 0; i < 6; i++) {
        PCA_SetChannel(i, 0, pos[i]);
    }
}
