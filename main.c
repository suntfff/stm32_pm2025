#include <stm32f10x.h>
#include <stdint.h>

volatile uint32_t sysTick_ms = 0;

void SysTick_Handler(void){
    sysTick_ms ++;
}

void delay_ms(uint32_t ms){
    uint32_t start = sysTick_ms;
    while ((sysTick_ms - start) < ms);
}

void SysTick_Init(void){
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);
}

int main(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    GPIOC->CRH &= ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14 | GPIO_CRH_CNF15 | GPIO_CRH_MODE15);
    GPIOC->CRH |= (GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1);
    GPIOC->ODR |= GPIO_ODR_ODR14 | GPIO_ODR_ODR15;
    SysTick_Init();
    delay_ms(100);
    if (!(GPIOC->IDR & GPIO_IDR_IDR14)) while (!(GPIOC->IDR & GPIO_IDR_IDR14));
    if (!(GPIOC->IDR & GPIO_IDR_IDR15)) while (!(GPIOC->IDR & GPIO_IDR_IDR15));
    uint32_t base_delay = 500;
    uint32_t freq_factor = 64;
    uint32_t max_factor = 64 * 64;
    uint32_t min_factor = 1;
    while(1){
        if (!(GPIOC->IDR & GPIO_IDR_IDR14)){
            delay_ms(50);
            if (!(GPIOC->IDR & GPIO_IDR_IDR14)){
                if (freq_factor < max_factor) freq_factor *= 2;
                while (!(GPIOC->IDR & GPIO_IDR_IDR14));
            }
        }
        if (!(GPIOC->IDR & GPIO_IDR_IDR15)){
            delay_ms(50);
            if (!(GPIOC->IDR & GPIO_IDR_IDR15)){
                if (freq_factor > min_factor) freq_factor /= 2;
                while (!(GPIOC->IDR & GPIO_IDR_IDR15));
            }
        }
        GPIOC->ODR ^= GPIO_ODR_ODR13;
        delay_ms((base_delay * 64)  / freq_factor);
    }
}