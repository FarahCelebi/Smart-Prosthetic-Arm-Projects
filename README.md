## Smart Prosthetic Arm

A six-project open hardware and firmware system for a surface EMG-controlled
prosthetic arm. The system reads muscle electrical signals from surface
electrodes, classifies hand gestures in real time, and drives a
servo-actuated prosthetic hand accordingly. Each project is a self-contained,
fully documented deliverable — schematic, PCB layout, Gerbers, and README —
designed to be fabricated and tested independently before integration.

## System Architecture

```
Surface electrodes
       │
       ▼
┌─────────────────┐
│  Project 1      │  EMG Acquisition Board
│  EMG signals    │  Dual-channel INA333 + ADS1115
│  → I²C out      │  Gain ~404×, 16-bit ADC
└────────┬────────┘
         │ I²C
         ▼
┌─────────────────┐
│  Project 2      │  Gesture Classifier Firmware
│  STM32 firmware │  Real-time EMG feature extraction
│  → gesture ID   │  + ML-based gesture classification
└────────┬────────┘
         │ I²C / UART
         ▼
┌─────────────────┐     ┌─────────────────┐
│  Project 3      │     │  Project 4      │
│  Servo Driver   │     │  BLE Module     │
│  PCA9685 PWM    │     │  nRF52840       │
│  6× servo out   │     │  Wireless telem │
└────────┬────────┘     └─────────────────┘
         │
         ▼
┌─────────────────┐
│  Project 5      │  IMU + EMG Sensor Fusion
│  ICM-42688-P    │  Accelerometer + gyroscope
│  + sensor fusion│  fused with EMG for robustness
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Project 6      │  Full Integration Capstone
│  All subsystems │  Complete assembled arm system
│  integrated     │  End-to-end validation
└─────────────────┘
```

## Projects

| # | Project | Type | Description |
|---|---|---|---|
| 01 | [EMG Acquisition Board](./Project-1-EMG-Acquisition) | PCB | Dual-channel surface EMG acquisition board using INA333 instrumentation amplifiers and an ADS1115 16-bit ADC. Conditions raw muscle signals through a 404× gain chain with high-pass filtering and anti-alias filtering before I²C output. |
| 02 | [Gesture Classifier Firmware](./Project-2-Gesture-Classifier) | Firmware | Real-time gesture classification firmware running on the STM32F411, fusing dual-channel EMG features with ICM-42688-P IMU data to classify six hand gestures. Outputs servo position commands over I²C and streams telemetry to the BLE module over UART. |
| 03 | [Servo Driver Board](./Project-3-Servo-Driver) | PCB | PCA9685-based 16-channel PWM controller driving six servo motors for prosthetic hand finger and wrist actuation over I²C. Separate 5–6V servo power rail with 2000µF bulk capacitance isolates motor switching transients from the logic supply. |
| 04 | [BLE Telemetry Module](./Project-4-BLE-Telemetry-Module) | PCB | Bare nRF52840-QIAA BLE 5.0 transceiver board with full RF front-end design including pi-network antenna matching and quarter-wave wire antenna. Accepts USB or LiPo power through a dual-input OR-ing diode circuit with a low-dropout XC6210 LDO. |
| 05 | [IMU + EMG Sensor Fusion](./Project-5-IMU+EMG-Sensor-Fusion) | PCB | ICM-42688-P 6-axis IMU board providing wrist orientation and motion data over high-speed SPI to complement EMG signals in the gesture classifier. Includes an I²C passthrough header matching the EMG board pinout for minimal wiring harness on the arm. |
| 06 | [Full Integration Capstone](./Project-6-Full-Integration-Capstone) | PCB + Firmware | Central hub board hosting the STM32F411 main MCU and routing power, I²C, SPI, and UART signals between all five subsystem boards through pinout-matched connectors. Runs the gesture classifier firmware and coordinates the complete prosthetic arm system from a single 3.7V LiPo cell. |

## Hardware Stack

| Component | Role |
|---|---|
| INA333 (×2) | Instrumentation amplifier |
| ADS1115 | 16-bit I²C ADC |
| TLV2372 | Second-stage op-amp |
| PCA9685PW | 16-channel PWM servo controller |
| nRF52840 | BLE 5.0 + ARM Cortex-M4 |
| ICM-42688-P | 6-axis IMU (accelerometer + gyroscope) |

## Tools

- KiCad (PCB design)
- GitHub Desktop (version control)
- Zephyr RTOS / STM32 HAL (firmware)