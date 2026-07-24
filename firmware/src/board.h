// pin map for FOC board

// MCU STM32G431CBT6
// core clock 170 MHz, HSE = 16MHz crystal
#pragma once

// LED: active high
#define LED_PWR_PORT GPIOC
#define LED_PWR_PIN 13
#define LED_STAT_PORT GPIOB
#define LED_STAT_PIN 6
#define LED_FAULT_PORT GPIOB
#define LED_FAULT_PIN 7

// UART debug: USART2, PA2=TX(AF7), PA3=RX(AF7)
#define UART_TX_PIN 2
#define UART_RX_PIN 3

// Encoder AS5047P: 
// SPI1 - PB3 = SCK
// PB4 = MISO
// PB5 = MOSI (AF5)
// PA15 = CS (GPIO)
#define ENC_CS_PORT GPIOA
#define ENC_CS_PIN 15

// DRV8313 control
// PWM: TIM1 CH1/2/3 -> PA8/9/10 (AF6) center-aligned 25kHz
#define PWM_FREQ_HZ 25000
// Enables (per phase, active high): EN_A=PB13, EN_B=PB14, EN_C=PB15
#define EN_A_PIN 13
#define EN_B_PIN 14
#define EN_C_PIN 15
// nSLEEP = PB2 (drive high to wake with 10k pulldown keeps driver off at reset)
#define DRV_SLEEP_PIN 2
// nRESET = PB11 (drive high for run)
#define DRV_RST_PIN 11
// nFAULT = PB10 (input, open-drain, external 10k pullup; low = fault)
#define DRV_FAULT_PIN 10

// Current sense: internal op-amps in PGA x16, low-side shunts 0.033R
// OPAMP1 VINP = PA1 (phase A) --> ADC1
// OPAMP2 VINP = PA7 (phase B) --> ADC2
// OPAMP3 VINP = PA0 (phase C) --> ADC3
#define SHUNT_OHMS 0.033f
#define PGA_GAIN 16.0f
#define VREF_V 3.3f
// amps per ADC count 12-bit: V/count / (gain * Rsh)
#define AMPS_PER_COUNT (VREF_V / 4096.0f / (GPA_GAIN * SHUNT_OHMS))

// CAN: FDCAN1, PA11=RX, PA12=TX (AF9) @ 500 kbit
#define CAN_NODE_ID 0x01

// Motor: iFLight GM4108H-120T
#define MOTOR_POLE_PAIRS 11
#define MOTOR_PHASE_R 11.0f // ohms, placeholder before bringup
#define CURRENT_LIMIT_A 0.5f // bring-up limit
#define VBUS_V 16.0f // placeholder before deciding power supply
