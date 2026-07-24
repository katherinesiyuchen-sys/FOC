// TIM1 center-aligned 3-phase PWM @ 25 kHz + DRV8313 control pins
// center-aligned: counter ramps 0->ARR->0
#include "g431.h"
#include "board.h"

extern uint32_t g_sysclk_hz;
static uint32_t s_arr;

void drv_init(void) {
    gpio_mode(GPIOB, DRV_SLEEP_PIN, 1, 0);
    gpio_mode(GPIOB, DRV_RST_PIN, 1, 0);
    gpio_mode(GPIOB, DRV_FAULT_PIN, 0, 0);         // input, external pullup
    gpio_mode(GPIOB, EN_A_PIN, 1, 0);
    gpio_mode(GPIOB, EN_B_PIN, 1, 0);
    gpio_mode(GPIOB, EN_C_PIN, 1, 0);
    gpio_set(GPIOB, DRV_SLEEP_PIN, 0);              // asleep until drv_wake()
    gpio_set(GPIOB, DRV_RST_PIN, 1);
    drv_enable_phases(0);
}

void drv_wake(void) { 
    gpio_set(GPIOB, DRV_SLEEP_PIN, 1); 
}
void drv_sleep(void) { 
    gpio_set(GPIOB, DRV_SLEEP_PIN, 0); 
}
int  drv_fault(void) { 
    return !gpio_get(GPIOB, DRV_FAULT_PIN); 
} // low = fault
void drv_enable_phases(int on) {
    gpio_set(GPIOB, EN_A_PIN, on);
    gpio_set(GPIOB, EN_B_PIN, on);
    gpio_set(GPIOB, EN_C_PIN, on);
}

void pwm_init(void) {
    gpio_mode(GPIOA, 8, 2, 6); // TIM1_CH1, AF6
    gpio_mode(GPIOA, 9, 2, 6); // TIM1_CH2
    gpio_mode(GPIOA, 10, 2, 6); // TIM1_CH3

    // center-aligned: effective PWM freq = fclk / (2*ARR)
    s_arr = g_sysclk_hz / (2u * PWM_FREQ_HZ); // 170e6/(2*25e3) = 3400
    TIM1->PSC = 0;
    TIM1->ARR = s_arr;
    TIM1->CCMR1 = (6u << 4) | (1u << 3)   // CH1 PWM mode 1 + preload
                | (6u << 12) | (1u << 11); // CH2
    TIM1->CCMR2 = (6u << 4) | (1u << 3);   // CH3
    TIM1->CCER  = (1u << 0) | (1u << 4) | (1u << 8); // CC1E CC2E CC3E
    TIM1->CR1   = (1u << 5)   // CMS = center-aligned mode 1
                | (1u << 7);  // ARPE
    TIM1->CR2   = (2u << 4);  // MMS = update -> TRGO (triggers ADC)
    TIM1->BDTR  = (1u << 15); // MOE — outputs live
    TIM1->DIER  = (1u << 0);  // update interrupt (control loop tick)
    TIM1->EGR   = 1; // load registers
    TIM1->CR1  |= 1; // count!
    nvic_enable(TIM1_UP_TIM16_IRQn);
}

// duties are 0.0..1.0
void pwm_set(float da, float db, float dc) {
    if (da < 0) da = 0; 
    if (da > 1) da = 1;
    if (db < 0) db = 0; 
    if (db > 1) db = 1;
    if (dc < 0) dc = 0; 
    if (dc > 1) dc = 1;
    TIM1->CCR1 = (uint32_t)(da * (float)s_arr);
    TIM1->CCR2 = (uint32_t)(db * (float)s_arr);
    TIM1->CCR3 = (uint32_t)(dc * (float)s_arr);
}
