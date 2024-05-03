#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "hardware/irq.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#define UART_TX_PIN 0
#define UART_RX_PIN 1

volatile int i = 0;
char zero_msg[100];

void on_uart_rx();

int main()
{
  // pin initializations
  stdio_init_all();
  uart_init(UART_ID, 2400);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

  uart_set_hw_flow(UART_ID, false, false);
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); // SET DATA FORMAT
  uart_set_fifo_enabled(UART_ID, false); // do it char by char

  // set up interrupt
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
  
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);
  uart_set_irq_enables(UART_ID, true, false);


  while (!stdio_usb_connected()){
    sleep_ms(100);
  }
  printf("Start!\n");

  while (1) {
    
    // read from usb
    int num_com = 0;
    scanf("%d", &num_com);

    // confirm received number
    printf("From computer: %d\r\n", num_com);

    // send over UART to Zero
    char uart_msg[100];
    sprintf(uart_msg, "%d\n", num_com);
    uart_puts(UART_ID, uart_msg);

    sleep_ms(250);
  }
}

void on_uart_rx(){
  while (uart_is_readable(UART_ID)){
    uint8_t cha = uart_getc(UART_ID);
    if (cha == '\n'){
      zero_msg[i] = 0;
      i = 0;
      printf("From Zero: %s\r\n", zero_msg);
    }
    else{
      zero_msg[i] = cha;
      i++;
    }
  }
}