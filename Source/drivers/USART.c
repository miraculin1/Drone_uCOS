#include "USART.h"
#include "shell.h"
#include <stdint.h>
#include <string.h>

// TODO: bad to have OS things here
// used to make sure usart are not called twice
OS_EVENT *uart1Sem;
static char uartSendBuf[MAXBUFLEN];
static char uartRecBuf[MAXBUFLEN];
char *puRecBuf;
char *const puRecBufInit = uartRecBuf;

uint8_t USART1CheckStatus(uint16_t sr) { return (USART1->SR & sr) == sr; }
void initUDMASend();
void initUDMARec();
void initUrec();

static void SetBaudRate(uint32_t baudR) {
  USART1->BRR &= 0;
  float tmp = (float)USART1_FREQ / (16 * baudR);
  USART1->BRR |= (int)tmp << 4;
  USART1->BRR |= ((int)(tmp * 16) - (((int)tmp) << 4));
}

// using PB6 TX
// PB7 RX
static void USARTPinInit() {
  RCC->AHB1ENR |= (0x1 << 1);

  // set to AF mode
  GPIOB->MODER &= ~(0x3 << (2 * 6));
  GPIOB->MODER &= ~(0x3 << (2 * 7));
  GPIOB->MODER |= (0x2 << (2 * 6));
  GPIOB->MODER |= (0x2 << (2 * 7));

  // set as default push-pull
  //

  // speed to high
  GPIOB->OSPEEDR |= (0x2 << (2 * 6));
  GPIOB->OSPEEDR |= (0x2 << (2 * 7));

  // set to pull up
  GPIOB->PUPDR &= ~(0x3 << (2 * 6));
  GPIOB->PUPDR &= ~(0x3 << (2 * 7));
  GPIOB->PUPDR |= (0x1 << (2 * 6));
  GPIOB->PUPDR |= (0x1 << (2 * 7));

  // set to AF7
  GPIOB->AFR[0] &= ~(0xf << (4 * 6));
  GPIOB->AFR[0] &= ~(0xf << (4 * 7));
  GPIOB->AFR[0] |= (0x7 << (4 * 6));
  GPIOB->AFR[0] |= (0x7 << (4 * 7));
}

void initUSART() {
  USARTPinInit();
  RCC->APB2ENR |= (0x1 << 4);

  // enable USART, UE bit
  USART1->CR1 |= (0x1 << 13);
  // define word lenth 8bit data 1bit stop
  USART1->CR1 &= ~(0x1 << 12);
  // define stop bit to 1
  USART1->CR2 &= ~(0x3 << 12);
  // DMA not enable

  // set baud rate
  // 16 over sample
  // 115200 baud rate
  // DIV 0x222 0x0e
  SetBaudRate(115200);
  initUDMASend();
  initUrec();
}

void USendByte(const uint8_t data) {
  // set TE to send idle
  USART1->CR1 |= (0x1 << 3);
  while (!USART1CheckStatus(USART_TC))
    ;
  USART1->DR = data;
  while (!USART1CheckStatus(USART_TC))
    ;
  // disable TE
  USART1->CR1 &= ~(0x1 << 3);
}
void USendStr(const char *str) {
  // set TE to send idle
  USART1->CR1 |= (0x1 << 3);
  while (!USART1CheckStatus(USART_TC))
    ;

  const char *tmp = str;
  while (*tmp != '\0') {
    USART1->DR = *tmp;
    while (!USART1CheckStatus(USART_TC))
      ;
    tmp++;
  }

  while (!USART1CheckStatus(USART_TC))
    ;
  // disable TE
  USART1->CR1 &= ~(0x1 << 3);
}

void initUDMASend() {
  RCC->AHB1ENR |= (0b1 << 22);

  // select channel 4 for UART1 tx
  DMA2_Stream7->CR |= (0b100 << 25);
  // selete priority
  DMA2_Stream7->CR |= (0b10 << 16);
  // data size are default

  // mem increace
  DMA2_Stream7->CR |= (0b1 << 10);
  // set mem->preph direction
  DMA2_Stream7->CR &= ~(0b11 << 6);
  DMA2_Stream7->CR |= (0b01 << 6);
  // set p Adress
  DMA2_Stream7->PAR = (uint32_t)&USART1->DR;
  uart1Sem = OSSemCreate(1);
  // enable tranfer complete interrupt
  DMA2_Stream7->CR |= (0b1 << 4);

  /* NVIC_SetPriority(DMA2_Stream7_IRQn, 0x1f); */
  NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

void UsendDMA(char *buf, const char *str, int len) {
  uint8_t ERORR;
  OSSemPend(uart1Sem, 0, &ERORR);
  // enable UART dam transmtte
  USART1->CR3 |= (0b1 << 7);
  // set TE to send idle
  USART1->CR1 |= (0x1 << 3);
  if (len > MAXBUFLEN - 1) {
    strcpy(buf, "[ERROR] uart buffer overflow\n");
  } else {
    memcpy(buf, str, len);
  }

  DMA2_Stream7->M0AR = (uint32_t)buf;
  DMA2_Stream7->NDTR = len;
  // clear TC flag in uart
  USART1->SR = ~(0b1 << 6);
  DMA2_Stream7->CR |= 0b1;
}

/* // [Abandon] using 3 interrupt to recieve data */
/* void initUDMARec() { */
  /* RCC->AHB1ENR |= (0b1 << 22); */
  /* // set channel 4 for rx */
  /* DMA2_Stream5->CR |= (0b100 << 25); */
  /* // priority highest */
  /* DMA2_Stream5->CR |= (0b11 << 16); */
  /* // mem increasing */
  /* DMA2_Stream5->CR |= (0b1 << 10); */
  /* // circular mode */
  /* DMA2_Stream5->CR |= (0b1 << 8); */
  /* // tc interrupt */
  /* DMA2_Stream5->CR |= (0b1 << 4); */
  /* // ht interrupt */
  /* DMA2_Stream5->CR |= (0b1 << 3); */
/*  */
  /* DMA2_Stream5->PAR = (uint32_t)&USART1->DR; */
  /* DMA2_Stream5->M0AR = (uint32_t)&uartRecBuf; */
/*  */
  /* // enable idle interrupt */
  /* USART1->CR1 |= (0b1 << 4); */
  /* NVIC_EnableIRQ(DMA2_Stream5_IRQn); */
  /* NVIC_EnableIRQ(USART1_IRQn); */
/* } */

void initUrec() {
  // enable rxne int
  USART1->CR1 |= (0b1 << 5);
  USART1->CR1 |= (0b1 << 2);
  puRecBuf = uartRecBuf;
  /* NVIC_SetPriority(USART1_IRQn, 0x13); */
  NVIC_EnableIRQ(USART1_IRQn);
}

// this handles share hardware resources
void os_writeByte(char byte) {
  OSSemPend(uart1Sem, 0, NULL);
  USendByte(byte);
  OSSemPost(uart1Sem);
}

// this handles share hardware resources
void os_witeStr(char *str, int len) { UsendDMA(uartSendBuf, str, len); }
