#include <stm32f10x.h>

/*Пины подключения дисплея SSD1306
PA5 -> CLK
PA7 -> DIN
PA4 -> CE
PA1 - > DC
PA2 -> RST
*/

void delay(uint32_t ticks){
    for (uint32_t i = 0; i < ticks; i++){
        __NOP();
    }
}

void delay_us(uint32_t us){
    delay(us * 8);
}

void SPI1_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 = 0;
    SPI1->CR1 |= SPI_CR1_CPOL
            | SPI_CR1_CPHA
            | SPI_CR1_MSTR
            | SPI_CR1_BR
            | SPI_CR1_SSM
            | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void SPI1_Write(uint8_t data){
    while(!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
}

uint8_t SPI1_Read(void){
    SPI1->DR = 0;
    while (!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}

void display_cmd(uint8_t cmd){
    GPIOA->ODR &= ~GPIO_ODR_ODR4;
    GPIOA->ODR &= ~GPIO_ODR_ODR1;
    delay_us(1);
    SPI1_Write(cmd);
    while (SPI1->SR & SPI_SR_BSY);
    GPIOA->ODR |= GPIO_ODR_ODR4;
}

void display_data(uint8_t data){
    GPIOA->ODR &= ~GPIO_ODR_ODR4;
    GPIOA->ODR |= GPIO_ODR_ODR1;
    delay_us(1);
    SPI1_Write(data);
    while (SPI1->SR & SPI_SR_BSY);
    GPIOA->ODR |= GPIO_ODR_ODR4;
}

void display_reset(void){
    GPIOA->ODR |= GPIO_ODR_ODR2;
    delay_us(10);
    GPIOA->ODR &= ~GPIO_ODR_ODR2;
    delay_us(3);
    GPIOA->ODR |= GPIO_ODR_ODR2;
    delay_us(3);
}

void display_init(void){
    display_reset();
    display_cmd(0xAE);
    display_cmd(0x20);
    display_cmd(0x00);
    display_cmd(0xA8);
    display_cmd(0x3F);
    display_cmd(0xD3);
    display_cmd(0x00);
    display_cmd(0x40);
    display_cmd(0xA1);
    display_cmd(0xC8);
    display_cmd(0xDA);
    display_cmd(0x12);
    display_cmd(0x81);
    display_cmd(0x7F);
    display_cmd(0xA4);
    display_cmd(0xA6);
    display_cmd(0xD5);
    display_cmd(0x80);
    display_cmd(0x8D);
    display_cmd(0x14);
    display_cmd(0xAF);
    delay_us(100000);
}

void display_test_pattern(void){
    for (uint16_t page = 0; page < 8; page ++){
        for (uint16_t col = 0; col < 128; col++){
            if (((col / 8) % 2) == (page % 2)){
                display_data(0xFF);
            }
            else{
                display_data(0x00);
            }
        }
    }
}

int main(void){
    RCC -> APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC -> APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC -> CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOC -> CRH |= GPIO_CRH_MODE13_1;
    GPIOA -> CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
    GPIOA -> CRL |= GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5;
    GPIOA -> CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
    GPIOA -> CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7;
    GPIOA -> CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
    GPIOA -> CRL |= GPIO_CRL_MODE4;
    GPIOA -> ODR |= GPIO_ODR_ODR4;
    GPIOA -> CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
    GPIOA -> CRL |= GPIO_CRL_MODE1;
    GPIOA -> CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA -> CRL |= GPIO_CRL_MODE2;
    GPIOA -> ODR |= GPIO_ODR_ODR2;
   
    SPI1_Init();
    display_init();
    display_test_pattern();
    while(1){
        GPIOC -> ODR ^= GPIO_ODR_ODR13;
        delay(1000000);
    }
}