#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define IODIR 0x00
#define OLAT_ADDR 0x0A
#define GPIO_ADDR 0x09
#define ADDR _u(0b0100000)

void init_pins();
void write_pin(uint8_t wregister, uint8_t wbyte);
void read_pin(uint8_t rregister, uint8_t *rdata);

int main()
{
  stdio_init_all();
  while (!stdio_usb_connected()){
    sleep_ms(100);
  }
  printf("Start!\r\n");

  // I2C Initializations
  i2c_init(i2c_default, 100 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

  // initialize MCP23008 pins
  init_pins();

  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (1){
    // turn on the LED and on-board LED (control)

    // test that it can read from chip
    uint8_t read_data[3];
    read_pin(GPIO_ADDR, read_data);

    uint8_t gp0 = (read_data[0] & 0b00000001);
    uint8_t gp7 = ((read_data[0] >> 7) | 0b00000001);
    printf("value on gp0: %d\r\n", gp0);
    printf("value on gp7: %d\r\n", gp7);

    if (gp0 == 0){
      write_pin(OLAT_ADDR, 0b10000000);
      gpio_put(LED_PIN, 1);
    }
    if (gp0 == 1){
      write_pin(OLAT_ADDR, 0b00000000);
      gpio_put(LED_PIN, 0);
    }
    sleep_ms(1000);
  }
}

void init_pins(){
  // initialize MCP23008 pins
  uint8_t pin_reg = IODIR;
  uint8_t pin_init = 0b01111111;
  uint8_t pin_bytes[2];
  pin_bytes[0] = pin_reg;
  pin_bytes[1] = pin_init;

  i2c_write_blocking(i2c_default, ADDR, pin_bytes, 2, false);  // true to keep master control of bus
}

void write_pin(uint8_t wregister, uint8_t wbyte){
  uint8_t write_bytes[2];
  write_bytes[0] = wregister;
  write_bytes[1] = wbyte;

  i2c_write_blocking(i2c_default, ADDR, write_bytes, 2, false);
}

void read_pin(uint8_t rregister, uint8_t *rdata){
  i2c_write_blocking(i2c_default, ADDR, &rregister, 1, false);
  i2c_read_blocking(i2c_default, ADDR, rdata, 1, false);
}