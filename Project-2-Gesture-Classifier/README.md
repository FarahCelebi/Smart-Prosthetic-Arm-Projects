# Gesture Classifier Firmware

Real-time hand gesture classification firmware running on an STM32
microcontroller, reading dual-channel EMG data from the EMG Acquisition
Board (Project 1) over I²C and outputting gesture commands to the Servo
Driver Board (Project 3) and BLE Module (Project 4).

## Status

🔄 **In development** — PCB hardware for Projects 1 and 3 is complete.
Firmware development begins after Project 4 (BLE Module) hardware is
finished, enabling full end-to-end testing across all subsystems.

## Planned Architecture

```
ADS1115 (I²C, 0x48)
       │
       ▼
EMG feature extraction
  - RMS per channel
  - Mean absolute value
  - Zero crossing rate
  - Waveform length
       │
       ▼
Gesture classification
  - Training data: logged via STM32 SDMMC Logger
  - Classifier: SVM or lightweight neural network
  - Output: gesture ID (rest, open, close, pinch, point, grip)
       │
       ▼
Command dispatch
  - Servo positions → PCA9685 via I²C (0x40)
  - Telemetry → nRF52840 via UART/SPI
```

## Gestures Targeted

| ID | Gesture |
|---|---|
| 0 | Rest (hand open, neutral) |
| 1 | Power grip (all fingers close) |
| 2 | Pinch (thumb + index) |
| 3 | Point (index finger extend) |
| 4 | Wrist pronate |
| 5 | Wrist supinate |

## Dependencies

- Project 1 — EMG Acquisition Board (hardware, complete)
- Project 3 — Servo Driver Board (hardware, complete)
- Project 4 — BLE Module (hardware, pending)
- STM32 HAL or Zephyr RTOS
- CMSIS-DSP for feature extraction

## Tools

- STM32CubeIDE or VS Code + Zephyr
- Python (training data collection and model training)
- scikit-learn or TensorFlow Lite for model generation
