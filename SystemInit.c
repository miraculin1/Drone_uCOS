// called by startup
// setup clock and flash
#include <stdint.h>

#define FPUEABLE 1
#define MCOENABLE 0

#define __IO volatile
#define __I volatile


#define RCC_BASE 0x40023800
#define SCB_BASE 0xE000ED00
#define RCC ((RCC_TypeDef *)RCC_BASE)
#define SCB ((SCB_Type *)SCB_BASE) /*!< SCB configuration struct           */

typedef struct
{
  __I  uint32_t CPUID;                   /*!< Offset: 0x000 (R/ )  CPUID Base Register                                   */
  __IO uint32_t ICSR;                    /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register                  */
  __IO uint32_t VTOR;                    /*!< Offset: 0x008 (R/W)  Vector Table Offset Register                          */
  __IO uint32_t AIRCR;                   /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register      */
  __IO uint32_t SCR;                     /*!< Offset: 0x010 (R/W)  System Control Register                               */
  __IO uint32_t CCR;                     /*!< Offset: 0x014 (R/W)  Configuration Control Register                        */
  __IO uint8_t  SHP[12];                 /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
  __IO uint32_t SHCSR;                   /*!< Offset: 0x024 (R/W)  System Handler Control and State Register             */
  __IO uint32_t CFSR;                    /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register                    */
  __IO uint32_t HFSR;                    /*!< Offset: 0x02C (R/W)  HardFault Status Register                             */
  __IO uint32_t DFSR;                    /*!< Offset: 0x030 (R/W)  Debug Fault Status Register                           */
  __IO uint32_t MMFAR;                   /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register                      */
  __IO uint32_t BFAR;                    /*!< Offset: 0x038 (R/W)  BusFault Address Register                             */
  __IO uint32_t AFSR;                    /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register                       */
  __I  uint32_t PFR[2];                  /*!< Offset: 0x040 (R/ )  Processor Feature Register                            */
  __I  uint32_t DFR;                     /*!< Offset: 0x048 (R/ )  Debug Feature Register                                */
  __I  uint32_t ADR;                     /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register                            */
  __I  uint32_t MMFR[4];                 /*!< Offset: 0x050 (R/ )  Memory Model Feature Register                         */
  __I  uint32_t ISAR[5];                 /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register                   */
       uint32_t RESERVED0[5];
  __IO uint32_t CPACR;                   /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register                   */
} SCB_Type;

typedef struct {
  __IO uint32_t CR; /*!< RCC clock control register, Address offset: 0x00 */
  __IO uint32_t
      PLLCFGR; /*!< RCC PLL configuration register, Address offset: 0x04 */
  __IO uint32_t
      CFGR; /*!< RCC clock configuration register, Address offset: 0x08 */
  __IO uint32_t CIR; /*!< RCC clock interrupt register, Address offset: 0x0C */
  __IO uint32_t
      AHB1RSTR; /*!< RCC AHB1 peripheral reset register, Address offset: 0x10 */
  __IO uint32_t
      AHB2RSTR; /*!< RCC AHB2 peripheral reset register, Address offset: 0x14 */
  __IO uint32_t
      AHB3RSTR; /*!< RCC AHB3 peripheral reset register, Address offset: 0x18 */
  uint32_t RESERVED0; /*!< Reserved, 0x1C */
  __IO uint32_t
      APB1RSTR; /*!< RCC APB1 peripheral reset register, Address offset: 0x20 */
  __IO uint32_t
      APB2RSTR; /*!< RCC APB2 peripheral reset register, Address offset: 0x24 */
  uint32_t RESERVED1[2]; /*!< Reserved, 0x28-0x2C */
  __IO uint32_t
      AHB1ENR; /*!< RCC AHB1 peripheral clock register, Address offset: 0x30 */
  __IO uint32_t
      AHB2ENR; /*!< RCC AHB2 peripheral clock register, Address offset: 0x34 */
  __IO uint32_t
      AHB3ENR; /*!< RCC AHB3 peripheral clock register, Address offset: 0x38 */
  uint32_t RESERVED2;    /*!< Reserved, 0x3C    */
  __IO uint32_t APB1ENR; /*!< RCC APB1 peripheral clock enable register, Address
                            offset: 0x40 */
  __IO uint32_t APB2ENR; /*!< RCC APB2 peripheral clock enable register, Address
                            offset: 0x44 */
  uint32_t RESERVED3[2]; /*!< Reserved, 0x48-0x4C */
  __IO uint32_t AHB1LPENR; /*!< RCC AHB1 peripheral clock enable in low power
                              mode register, Address offset: 0x50 */
  __IO uint32_t AHB2LPENR; /*!< RCC AHB2 peripheral clock enable in low power
                              mode register, Address offset: 0x54 */
  __IO uint32_t AHB3LPENR; /*!< RCC AHB3 peripheral clock enable in low power
                              mode register, Address offset: 0x58 */
  uint32_t RESERVED4;      /*!< Reserved, 0x5C      */
  __IO uint32_t APB1LPENR; /*!< RCC APB1 peripheral clock enable in low power
                              mode register, Address offset: 0x60 */
  __IO uint32_t APB2LPENR; /*!< RCC APB2 peripheral clock enable in low power
                              mode register, Address offset: 0x64 */
  uint32_t RESERVED5[2];   /*!< Reserved, 0x68-0x6C   */
  __IO uint32_t
      BDCR; /*!< RCC Backup domain control register, Address offset: 0x70 */
  __IO uint32_t
      CSR; /*!< RCC clock control & status register, Address offset: 0x74 */
  uint32_t RESERVED6[2];    /*!< Reserved, 0x78-0x7C    */
  __IO uint32_t SSCGR;      /*!< RCC spread spectrum clock generation register,
                               Address offset: 0x80 */
  __IO uint32_t PLLI2SCFGR; /*!< RCC PLLI2S configuration register, Address
                               offset: 0x84 */
  __IO uint32_t PLLSAICFGR; /*!< RCC PLLSAI configuration register, Address
                               offset: 0x88 */
  __IO uint32_t DCKCFGR;    /*!< RCC Dedicated Clocks configuration register,
                               Address offset: 0x8C */
  __IO uint32_t CKGATENR;      /*!< RCC Clocks Gated Enable Register,                            Address offset: 0x90 */ /* Only for STM32F412xG, STM32413_423xx and STM32F446xx devices */
  __IO uint32_t DCKCFGR2;      /*!< RCC Dedicated Clocks configuration register 2,               Address offset: 0x94 */ /* Only for STM32F410xx, STM32F412xG, STM32413_423xx and STM32F446xx devices */

} RCC_TypeDef;

#define CLKSETTRIES 0xffff
#define HSERDYMSK 0x00020000
#define PLLRDYMSK 0x02000000
#define SYSPLLMSK 0x00000008

#if MCOENABLE == 1
#define RCC_AHB1ENR *(uint32_t *)(RCC_BASE + 0x30)
#define GPIOC_BASE (uint32_t)0x40020800
#define GPIOC_MODER *(uint32_t *)(GPIOC_BASE + 0x00)
#define GPIOC_OSPEEDR *(uint32_t *)(GPIOC_BASE + 0x08)

void MCOenable() {
  RCC_CFGR &= ~(uint32_t)(0x3 << 30); // MCO2 choose PLL
  RCC_CFGR |= (uint32_t)(0x3 << 30);
  RCC_CFGR |= (uint32_t)(1 << 29);
  RCC_AHB1ENR &= ~(uint32_t)(1 << 2); // GPIOC enable
  RCC_AHB1ENR |= (uint32_t)(1 << 2);
  GPIOC_MODER &= ~(uint32_t)(0x3 << 18); // GPIOC_9
  GPIOC_MODER |= (uint32_t)(1 << 19);
  GPIOC_OSPEEDR &= ~(uint32_t)(0x3 << 18); // GPIOC_9 50MHz speed
  GPIOC_OSPEEDR |= (uint32_t)(1 << 19);
}
#endif

void SystemInit() {
  volatile uint32_t RDY;
  // reset all regs default num
  RCC->CR |= 0x00000081;
  RCC->PLLCFGR = 0x24003010;
  RCC->CFGR = 0x00000000;

  // for now eable the FPU may cause problem in "stacking"
#if FPUEABLE
  // enable FPU
  SCB->CPACR |= (0b11 << 20);
  SCB->CPACR |= (0b11 << 22);
#endif
  uint32_t try;
  // enable the HSE
  RCC->CR |= (0b1 << 16);

  // check if HSE is rdy
  try = 0;
  RDY = RCC->CR & HSERDYMSK;
  while (RDY) {
    if (try >= CLKSETTRIES) {
      return;
    }
    RDY = RCC->CR & HSERDYMSK;
    try++;
  }

  // config pll thingy

  // select HSE
  RCC->PLLCFGR |= (0x1 << 22);
  // set M = 4
  RCC->PLLCFGR &= ~(0x3f);
  RCC->PLLCFGR |= (4 << 0);
  // N = 84
  RCC->PLLCFGR &= ~(0x1ff << 6);
  RCC->PLLCFGR |= (84 << 6);
  // P = 2
  RCC->PLLCFGR &= ~(0b11 << 16);
  // Q = 4
  RCC->PLLCFGR &= ~(0xf << 24);
  RCC->PLLCFGR |= (4 << 24);

  // APB2
  RCC->CFGR &= ~(0b111 << 13);
  // APB1
  RCC->CFGR &= ~(0b111 << 10);
  RCC->CFGR &= ~(0b100 << 10);
  // AHB
  RCC->CFGR &= ~(0b0000 << 4);

  // enable PLL
  RCC->CR |= (0x1 << 24);

  RDY = RCC->CR & PLLRDYMSK;
  while ((RDY) == 0) {
    RDY = RCC->CR & PLLRDYMSK;
  }

  /* // switch */
  RCC->CFGR |= (0b10 << 0);

  RDY = (RCC->CR ^ SYSPLLMSK) & 0x0000000c;
  while (RDY == 0) {
    RDY = (RCC->CR ^ SYSPLLMSK) & 0x0000000c;
  }

#if MCOENABLE == 1
  MCOenable();
#endif
}
