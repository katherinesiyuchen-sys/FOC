// minimal register definitions for peripherals this firmware uses
// if later import into CubeIDE, you can swap this for the CMSIS device header with minimal changes

#pragma once
#include <stdint.h>

#define __IO volatile

// Memory map STM32G4
#define PERIPH_BASE     0x40000000u
#define RCC_BASE        0x40021000u
#define FLASE_R_BASE    0x40022000u
#define PWR_BASE        0x40007000u
#define GPIOA_BASE      0x48000000u
#define GPIOB_BASE      0x48000400u
#define GPIOC_BASE      0x48000800u
#define USART2_BASE     0x40004400u
#define SPI1_BASE       0x40013000u
#define TIM1_BASE       0x40012C00u
#define ADC12_BASE      0x50000000u   // ADC1 @ +0x000, ADC2 @ +0x100, common @ +0x300
#define OPAMP_BASE      0x40010300u
#define FDCAN1_BASE     0x40006400u
#define FDCAN_RAM_BASE  0x4000A400u
#define NVIC_ISER       ((__IO uint32_t*)0xE000E100u)
#define SYSTICK_BASE    0xE000E010u

typedef struct { __IO uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRL, AFRH, BRR; } GPIO_t;
#define GPIOA ((GPIO_t*)GPIOA_BASE)
#define GPIOB ((GPIO_t*)GPIOB_BASE)
#define GPIOC ((GPIO_t*)GPIOC_BASE)

typedef struct {
    __IO uint32_t CR, ICSCR, CFGR, PLLCFGR, RES0, RES1, CIER, CIFR, CICR, RES2,
                AHB1RSTR, AHB2RSTR, AHB3RSTR, RES3, APB1RSTR1, APB1RSTR2, APB2RSTR, RES4,
                AHB1ENR, AHB2ENR, AHB3ENR, RES5, APB1ENR1, APB1ENR2, APB2ENR, RES6,
                AHB1SMENR, AHB2SMENR, AHB3SMENR, RES7, APB1SMENR1, APB1SMENR2, APB2SMENR, RES8,
                CCIPR, RES9, BDCR, CSR, CRRCR, CCIPR2;
} RCC_t;
#define RCC ((RCC_t*)RCC_BASE)

typedef struct { __IO uint32_t ACR; } FLASH_t;
#define FLASH_R ((FLASH_t*)FLASH_R_BASE)

typedef struct { __IO uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR, RDR, TDR, PRESC; } USART_t;
#define USART2 ((USART_t*)USART2_BASE)

typedef struct { __IO uint32_t CR1, CR2, SR, DR, CRCPR, RXCRCR, TXCRCR; } SPI_t;
#define SPI1 ((SPI_t*)SPI1_BASE)

typedef struct {
  __IO uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2, CCER, CNT, PSC, ARR, RCR,
                CCR1, CCR2, CCR3, CCR4, BDTR, CCR5, CCR6, CCMR3, DTR2, ECR, TISEL, AF1, AF2;
} TIM_t;
#define TIM1 ((TIM_t*)TIM1_BASE)

typedef struct {
  __IO uint32_t ISR, IER, CR, CFGR, CFGR2, SMPR1, SMPR2, RES0, TR1, TR2, TR3, RES1,
                SQR1, SQR2, SQR3, SQR4, DR, RES2, RES3, JSQR, RES4[4],
                OFR1, OFR2, OFR3, OFR4, RES5[4], JDR1, JDR2, JDR3, JDR4,
                RES6[4], AWD2CR, AWD3CR, RES7[2], DIFSEL, CALFACT, RES8[2], GCOMP;
} ADC_t;
#define ADC1 ((ADC_t*)(ADC12_BASE + 0x000))
#define ADC2 ((ADC_t*)(ADC12_BASE + 0x100))
typedef struct { __IO uint32_t CSR, RES0, CCR, CDR; } ADC_Common_t;
#define ADC12_COMMON ((ADC_Common_t*)(ADC12_BASE + 0x300))

typedef struct { __IO uint32_t CSR; __IO uint32_t TCMR; } OPAMP_chan_t; // simplified
#define OPAMP1_CSR (*(__IO uint32_t*)(OPAMP_BASE + 0x00))
#define OPAMP2_CSR (*(__IO uint32_t*)(OPAMP_BASE + 0x04))
#define OPAMP3_CSR (*(__IO uint32_t*)(OPAMP_BASE + 0x08))

// Interrupt numbers (STM32G431) 
#define TIM1_UP_TIM16_IRQn 25
#define ADC1_2_IRQn 18

static inline void nvic_enable(int irqn) { NVIC_ISER[irqn >> 5] = 1u << (irqn & 31); }

// SysTick
typedef struct { __IO uint32_t CTRL, LOAD, VAL, CALIB; } SYSTICK_t;
#define SYSTICK ((SYSTICK_t*)SYSTICK_BASE)
