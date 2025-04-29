// /***************************************************************************/ /**
//    文件: main.c
//    作者: Zhengyu https://gzwelink.taobao.com
//    版本: V1.0.0
//    时间: 20220401
//    平台:MINI-F407VET6

//  *******************************************************************************/
#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"
#include "systick.h"

/* 从机接收 */
#define I2C0_OWN_ADDRESS7 0x82
uint8_t i2c_receiver[16];

void rcu_config(void);
void gpio_config(void);
void i2c_config(void);

#define ARRAYNUM(arr_nanme) (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE (ARRAYNUM(txbuffer) - 1)

uint8_t txbuffer[] = "\n\rUSART interrupt test\n\r";
uint8_t rxbuffer[32];
uint8_t tx_size = TRANSMIT_SIZE;
uint8_t rx_size = 32;
__IO uint8_t txcount = 0;
__IO uint16_t rxcount = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
  int i;

  /* configure USART */
  // gd_eval_com_init(EVAL_COM0);
  // printf("\r\n I2C Start");

  /* configure RCU */
  rcu_config();
  /* configure GPIO */
  gpio_config();
  /* configure I2C */
  i2c_config();

  /* USART DEMO */
  /* USART interrupt configuration */
  nvic_irq_enable(USART0_IRQn, 0, 0);
  /* configure COM0 */
  gd_eval_com_init(EVAL_COM0);
  /* enable USART TBE interrupt */
  usart_interrupt_enable(USART0, USART_INT_TBE);

  /* wait until USART send the transmitter_buffer */
  while (txcount < tx_size)
    ;

  while (RESET == usart_flag_get(USART0, USART_FLAG_TC))
    ;

  usart_interrupt_enable(USART0, USART_INT_RBNE);

  /* wait until USART receive the receiver_buffer */
  while (rxcount < rx_size)
    ;
  if (rxcount == rx_size)
  {
    printf("\n\rUSART receive successfully!\n\r");
  }

  /* I2C slave receiver */
  i = 0;
  /* wait until ADDSEND bit is set */
  while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
    ;
  /* clear ADDSEND bit */
  i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

  for (i = 0; i < 16; i++)
  {
    /* wait until the RBNE bit is set */
    while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
      ;
    /* read a data byte from I2C_DATA */
    i2c_receiver[i] = i2c_data_receive(I2C0);
  }
  /* wait until the STPDET bit is set */
  while (!i2c_flag_get(I2C0, I2C_FLAG_STPDET))
    ;
  /* clear the STPDET bit */
  i2c_enable(I2C0);

  for (i = 0; i < 16; i++)
  {
    printf("0x%02X ", i2c_receiver[i]);
  }

  while (1)
  {
  }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
  /* enable GPIOB clock */
  rcu_periph_clock_enable(RCU_GPIOB);
  /* enable I2C0 clock */
  rcu_periph_clock_enable(RCU_I2C0);
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
  /* I2C0 GPIO ports */
  /* connect PB6 to I2C0_SCL */
  gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_6);
  /* connect PB7 to I2C0_SDA */
  gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_7);

  /* configure I2C0 GPIO */
  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}

/*!
    \brief      configure the I2C0 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
  /* configure I2C clock */
  i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
  /* configure I2C address */
  i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
  /* enable I2C0 */
  i2c_enable(I2C0);
  /* enable acknowledge */
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

// /* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
  usart_data_transmit(EVAL_COM0, (uint8_t)ch);
  while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE))
    ;
  return ch;
}

/* 主机发送-主机接收 */
// 0x72
// #define I2C0_OWN_ADDRESS7 0x50
// #define I2C0_SLAVE_ADDRESS7 0x82
// uint8_t i2c_transmitter[16];

// #define SLAVE_ADDR 0x32 << 1 // 从机地址左移1位（HAL库要求）
// uint8_t send_data[] = {0xAA, 0x55};

// void rcu_config(void);
// void gpio_config(void);
// void i2c_config(void);

// // 发送数据函数
// void I2C_SendData(uint8_t *data, uint8_t len)
// {
//   i2c_master_addressing(I2C0, SLAVE_ADDR, I2C_TRANSMITTER); // 设定从机地址和传输方向
//   i2c_start_on_bus(I2C0);
//   while (len--)
//   {
//     i2c_data_transmit(I2C0, *data++);
//     while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
//       ; // 等待数据发送完成
//   }
//   i2c_stop_on_bus(I2C0);
// }

// void I2C_DEMO(void)
// {
//   int i;
//   // 填充数组中的每个元素被设置为0x80开始的连续值，共16个字节的数据
//   for (i = 0; i < 16; i++)
//   {
//     i2c_transmitter[i] = i + 0x80;
//   }

//   /* wait until I2C bus is idle */
//   while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
//     ;
//   /* send a start condition to I2C bus */
//   i2c_start_on_bus(I2C0);
//   /* wait until SBSEND bit is set */
//   while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
//     ;
//   /* send slave address to I2C bus 发送从设备地址并指定传输方向为发送 */
//   i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);
//   /* wait until ADDSEND bit is set */
//   while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
//     ;
//   /* clear ADDSEND bit */
//   i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
//   /* wait until the transmit data buffer is empty */
//   while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
//     ;

//   // 将发送数组中的每个字节数据通过I2C总线发送给从设备，每发送一个数据都会等待缓冲区变为空闲状态
//   for (i = 0; i < 16; i++)
//   {
//     /* data transmission */
//     i2c_data_transmit(I2C0, i2c_transmitter[i]);
//     /* wait until the TBE bit is set */
//     while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
//       ;
//   }
//   /* send a stop condition to I2C bus */
//   i2c_stop_on_bus(I2C0);
//   while (I2C_CTL0(I2C0) & I2C_CTL0_STOP)
//     ;
// }

// /*!
//     \brief      main function
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// int main(void)
// {

//   systick_config();                                                             // 配置系统主频168M,外部8M晶振,配置在#define __SYSTEM_CLOCK_168M_PLL_8M_HXTAL        (uint32_t)(168000000)
//   rcu_periph_clock_enable(RCU_GPIOB);                                           // 使能GPIOB时钟
//   gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);           // PB4配置成输出
//   gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4); // PB4配置成推挽输出，50M速度

//   /* configure RCU */
//   rcu_config();
//   /* configure GPIO */
//   gpio_config();
//   /* configure I2C */
//   i2c_config();

//   // I2C_SendData(send_data, sizeof(send_data));
//   I2C_DEMO();

//   /* infinite loop */
//   while (1)
//   {

//     gpio_bit_set(GPIOB, GPIO_PIN_4);   // PB4输出高电平
//     delay_1ms(100);                    // 等待100ms
//     gpio_bit_reset(GPIOB, GPIO_PIN_4); // PB4输出低电平
//     delay_1ms(100);
//   }
// }

// /*!
//     \brief      enable the peripheral clock
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// void rcu_config(void)
// {
//   /* enable GPIOB clock */
//   rcu_periph_clock_enable(RCU_GPIOB);
//   /* enable I2C0 clock */
//   rcu_periph_clock_enable(RCU_I2C0);
// }

// /*!
//     \brief      configure the GPIO ports
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// void gpio_config(void)
// {
//   /* I2C0 GPIO ports */
//   /* connect PB6 to I2C0_SCL */
//   gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_6);
//   /* connect PB7 to I2C0_SDA */
//   gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_7);

//   /* configure I2C0 GPIO */
//   gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
//   gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
//   gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
//   gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
// }

// /*!
//     \brief      configure the I2C0 interface
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// void i2c_config(void)
// {
//   /* configure I2C clock */
//   i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
//   /* configure I2C address */
//   i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
//   /* enable I2C0 */
//   i2c_enable(I2C0);
//   /* enable acknowledge */
//   i2c_ack_config(I2C0, I2C_ACK_ENABLE);
// }
