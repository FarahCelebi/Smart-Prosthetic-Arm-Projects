#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "emg.h"
#include "imu.h"
#include <stdint.h>

typedef enum {
    GESTURE_REST     = 0,
    GESTURE_GRIP     = 1,
    GESTURE_PINCH    = 2,
    GESTURE_POINT    = 3,
    GESTURE_PRONATE  = 4,
    GESTURE_SUPINATE = 5
} Gesture;

Gesture Classifier_Run(EMGFeatures *emg, IMUData *imu);

#ifdef __cplusplus
}
#endif

#endif // CLASSIFIER_H
