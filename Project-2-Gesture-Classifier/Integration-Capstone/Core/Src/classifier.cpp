#include "classifier.h"

Gesture Classifier_Run(EMGFeatures *emg, IMUData *imu) {

    // Rest — both channels quiet
    if (emg->rms_ch1 < 200.0f && emg->rms_ch2 < 200.0f) {
        return GESTURE_REST;
    }

    // Pronate — wrist rolling inward (gyro_z positive)
    if (imu->gyro_z > 50.0f && emg->rms_ch1 < 500.0f) {
        return GESTURE_PRONATE;
    }

    // Supinate — wrist rolling outward (gyro_z negative)
    if (imu->gyro_z < -50.0f && emg->rms_ch1 < 500.0f) {
        return GESTURE_SUPINATE;
    }

    // Pinch — ch2 dominant, ch1 moderate
    if (emg->rms_ch2 > emg->rms_ch1 * 1.5f && emg->rms_ch2 > 400.0f) {
        return GESTURE_PINCH;
    }

    // Point — ch1 dominant, ch2 quiet
    if (emg->rms_ch1 > emg->rms_ch2 * 1.5f && emg->rms_ch1 > 400.0f) {
        return GESTURE_POINT;
    }

    // Grip — both channels active
    if (emg->rms_ch1 > 400.0f && emg->rms_ch2 > 400.0f) {
        return GESTURE_GRIP;
    }

    return GESTURE_REST;
}
