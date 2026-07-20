# Gesture-Classifier-Firmware

Real-time hand gesture classification firmware for the Smart Prosthetic Arm
system, designed as Project 2 of 6. Running on the STM32F411RETx integration
board, the firmware reads dual-channel EMG signals from the ADS1115 ADC over
I²C, reads 6-axis inertial data from the ICM-42688-P IMU over SPI, fuses
both sensor streams to classify one of six hand gestures, drives the PCA9685
servo controller over I²C to actuate the prosthetic hand, and streams
telemetry to the nRF52840 BLE module over UART — all in a continuous
real-time loop.

## Repository Structure

```
Project-2-Gesture-Classifier/
├── README.md
└── Integration-Capstone/
    └── Core/
        ├── Inc/
        │   ├── emg.h
        │   ├── imu.h
        │   ├── classifier.h
        │   ├── servo.h
        │   └── ble.h
        └── Src/
            ├── main.c
            ├── emg.cpp
            ├── imu.cpp
            ├── classifier.cpp
            ├── servo.cpp
            └── ble.cpp
```

## System Architecture

```
ADS1115 (I²C1, 0x48)          ICM-42688-P (SPI1)
        │                              │
        ▼                              ▼
  emg.cpp                        imu.cpp
  - Read CH0, CH1                - Read accel x/y/z
  - Collect 200-sample window    - Read gyro x/y/z
  - Compute RMS, MAV             - Scale to g and °/s
        │                              │
        └──────────┬───────────────────┘
                   ▼
            classifier.cpp
            - Threshold-based gesture ID
            - 6 gestures: REST, GRIP, PINCH,
              POINT, PRONATE, SUPINATE
                   │
        ┌──────────┴──────────┐
        ▼                     ▼
   servo.cpp              ble.cpp
   PCA9685 (I²C1, 0x40)  nRF52840 (UART1)
   6-channel PWM          Telemetry packet
   finger/wrist control   G, E1, E2, AX, AY, AZ, GZ
```

## Gesture Definitions

| ID | Gesture | EMG Signature | IMU Signature |
|---|---|---|---|
| 0 | REST | Both channels quiet (<200 RMS) | Any |
| 1 | GRIP | Both channels active (>400 RMS) | Neutral |
| 2 | PINCH | CH2 dominant (>1.5× CH1) | Neutral |
| 3 | POINT | CH1 dominant (>1.5× CH2) | Neutral |
| 4 | PRONATE | CH1 moderate | Gyro Z > +50 °/s |
| 5 | SUPINATE | CH1 moderate | Gyro Z < -50 °/s |

## Design Notes

**Two-phase classifier architecture:** Phase 1 (this implementation) uses a
threshold-based classifier — fast, deterministic, and requires no training
data. EMG features (RMS and MAV per channel) are computed over a 200-sample
window at 860 SPS from the ADS1115, giving a feature update rate of
approximately 4Hz. IMU data is sampled once per gesture cycle. Phase 2 will
replace the threshold classifier with a support vector machine or lightweight
neural network trained on labelled EMG/IMU data collected using the SDMMC
Logger board (Project 3), improving classification accuracy for ambiguous
gestures.

**Sensor fusion rationale:** Surface EMG signals alone cannot reliably
distinguish pronation from supination or wrist-neutral gestures, since the
forearm muscle activation patterns overlap. The ICM-42688-P gyroscope Z-axis
provides the wrist rotation signal that resolves this ambiguity — the IMU
acts as a tie-breaker for gestures that EMG features cannot separate cleanly.

**EMG feature extraction:** Two features are computed per channel per window.
RMS (root mean square) captures signal energy and correlates with muscle
contraction force. MAV (mean absolute value) is computationally cheaper and
tracks signal amplitude. Both are computed in emg.cpp using integer
accumulation before a single sqrtf call, minimising floating point load on
the Cortex-M4F.

**ADS1115 sampling:** The ADS1115 is operated in single-shot mode at 860
SPS. Each channel conversion is triggered by writing the config register,
followed by a 2ms delay and a register read. With 200 samples per channel
per window, total EMG acquisition time is approximately 800ms per cycle.
Future optimisation will use the ALERT/RDY pin for interrupt-driven sampling,
removing the HAL_Delay calls and enabling higher effective sample rates.

**ICM-42688-P interface:** The IMU is read over SPI1 at 8MHz. A burst read
of 12 bytes starting at the accelerometer output register retrieves all six
axes in one transaction. Raw 16-bit values are scaled to g (accel,
±2g range, 16384 LSB/g) and degrees per second (gyro, ±250°/s range,
131 LSB/°/s).

**PCA9685 servo control:** The PCA9685 is configured for 50Hz PWM output
using an internal prescaler value of 121. Each servo channel is set using
a pulse width between 205 counts (~1ms, full open) and 410 counts (~2ms,
full close), with 307 counts (~1.5ms) as the neutral position. Six channels
(LED0–LED5) drive one servo per finger and wrist.

**BLE telemetry format:** Each telemetry packet is a null-terminated ASCII
string transmitted over UART1 at 115200 baud to the nRF52840 BLE module.
Format: `G:%d,E1:%d,E2:%d,AX:%d,AY:%d,AZ:%d,GZ:%d\r\n` where EMG RMS
values are integer counts, accelerometer values are scaled ×100 (cm/s²
units), and gyroscope Z is scaled ×10 (0.1°/s units). Float formatting
is avoided as `--specs=nano.specs` disables float printf support by default
in the ARM GCC nano libc.

**Build system:** The project is generated by STM32CubeMX and built from
the Terminal using the GNU ARM toolchain bundled with STM32CubeIDE. C HAL
files are compiled with arm-none-eabi-gcc; application modules (emg.cpp,
imu.cpp, classifier.cpp, servo.cpp, ble.cpp) and main.c are compiled with
arm-none-eabi-g++ to enable C++14 features and resolve C/C++ Thumb
interworking. The final ELF is linked with arm-none-eabi-g++.

## Build Instructions

```bash
# Add toolchain to PATH (adjust version as needed)
export PATH="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/\
com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.14.3.rel1.\
macosaarch64_1.0.0.202602081740/tools/bin:$PATH"

cd Integration-Capstone/Debug
make all
```

## Flash Instructions

Connect an ST-LINK V2 to the SWD header (J3 on the Integration Capstone
Board) and use STM32CubeProgrammer or the CubeIDE debugger to flash
`Integration-Capstone.elf`.

## Binary Size (Phase 1)

| Section | Size |
|---|---|
| text (flash) | 47,824 bytes |
| data (initialized) | 332 bytes |
| bss (zero-initialized RAM) | 9,388 bytes |

## Hardware Dependencies

| Board | Role | Interface |
|---|---|---|
| EMG Acquisition Board | EMG signal source | I²C1 (0x48) |
| IMU Sensor Fusion Board | Motion data | SPI1 + PA4 CS |
| Servo Driver Board | Hand actuation | I²C1 (0x40) |
| BLE Module | Wireless telemetry | UART1 115200 |

## Part of

Smart Prosthetic Arm — Project 2 of 6

## Tools

- STM32CubeMX (peripheral configuration)
- STM32CubeIDE (project structure)
- GNU ARM Toolchain 14.3 (compilation)
- C++14
