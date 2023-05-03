#include "Includes.h"

uint8_t USART1CheckStatus(uint16_t sr) { return (USART1_SR & sr) == sr; }

static void SetBaudRate(uint32_t baudR) {
  USART1_BRR &= ~(0xffff);
  float tmp = (float)USART1_FREQ / (16 * baudR);
  USART1_BRR |= (int)tmp << 4;
  USART1_BRR |= ((int)(tmp * 16) - (((int)tmp) << 4));
}

// using PB6 TX
// PB7 RX
static void USARTPinInit() {
  RCC_AHB1 |= (0x1 << 1);

  // set to AF mode
  GPIOB_MODER &= ~(0x3 << (2 * 6));
  GPIOB_MODER &= ~(0x3 << (2 * 7));
  GPIOB_MODER |= (0x2 << (2 * 6));
  GPIOB_MODER |= (0x2 << (2 * 7));

  // set as default push-pull
  //

  // speed to high
  GPIOB_OSPEEDR |= (0x2 << (2 * 6));
  GPIOB_OSPEEDR |= (0x2 << (2 * 7));

  // set to pull up
  GPIOB_PUPDR &= ~(0x3 << (2 * 6));
  GPIOB_PUPDR &= ~(0x3 << (2 * 7));
  GPIOB_PUPDR |= (0x1 << (2 * 6));
  GPIOB_PUPDR |= (0x1 << (2 * 7));

  // set to AF7
  GPIOB_AF_L &= ~(0xf << (4 * 6));
  GPIOB_AF_L &= ~(0xf << (4 * 7));
  GPIOB_AF_L |= (0x7 << (4 * 6));
  GPIOB_AF_L |= (0x7 << (4 * 7));
}

void initUSART() {
  USARTPinInit();
  RCC_APB2 |= (0x1 << 4);

  // enable USART, UE bit
  USART1_CR1 |= (0x1 << 13);
  // define word lenth 8bit data 1bit stop
  USART1_CR1 |= (0x1 << 12);
  // define stop bit to 1
  USART1_CR2 &= ~(0x3 << 12);
  // DMA not enable

  // set baud rate
  // 16 over sample
  // 9600 baud rate
  // DIV 0x222 0x0e
  SetBaudRate(9600);
}

void USendByte(uint8_t data) {
  // set TE to send idle
  USART1_CR1 |= (0x1 << 3);
  while (!USART1CheckStatus(USART_TC))
    ;
  USART1_DR = data;
  while (!USART1CheckStatus(USART_TC))
    ;
  // disable TE
  USART1_CR1 &= ~(0x1 << 3);
}

void USendStr(char *str) {
  // set TE to send idle
  USART1_CR1 |= (0x1 << 3);
  while (!USART1CheckStatus(USART_TC))
    ;

  char *tmp = str;
  while (*tmp != '\0') {
    USART1_DR = *tmp;
    while (!USART1CheckStatus(USART_TC))
      ;
    tmp++;
  }

  while (!USART1CheckStatus(USART_TC))
    ;
  // disable TE
  USART1_CR1 &= ~(0x1 << 3);
}

void UReceByte(uint8_t *Byte) {
  // recive init
  //
  USART1_CR1 |= (0x1 << 2);
  // set NVIC

  *Byte = USART1_DR;
  USART1_CR1 &= ~(0x1 << 2);
}

void USendInt(uint32_t data) {
  char str[11];
  uint8_t now = 0;
  do {
    str[now] = '0' + data % 10;
    data /= 10;
    now++;
  } while (data > 0);
  str[now] = '\0';

  char res[now + 1];
  for (int i = 0; i < now; i++) {
    res[now - i - 1] = str[i];
  }
  res[now] = '\0';

  USendStr(res);
}
