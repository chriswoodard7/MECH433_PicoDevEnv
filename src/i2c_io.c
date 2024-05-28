#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SDA_PIN 12
#define SCL_PIN 13

#define IODIR 0x00
#define OLAT 0x0A
#define READ_ADDR _u(0b01000001)
#define WRITE_ADDR _u(0b01000000)
#define ADDR _u(0b0100000)

void init_pins();
void write_pin(uint8_t wregister, uint8_t wbyte);

int main()
{
  stdio_init_all();

  // I2C Initializations
  i2c_init(i2c_default, 100 * 1000);
  // gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  // gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

  // initialize MCP23008 pins
  init_pins();

  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (1){
    // turn on the LED and on-board LED (control)
    write_pin(OLAT, 0b1000000);
    gpio_put(LED_PIN, 1);

    //turn off LEDs
    write_pin(OLAT, 0b0000000);
    gpio_put(LED_PIN, 0);
    sleep_ms(1000);
  }
}

void init_pins(){
  // initialize MCP23008 pins
  uint8_t pin_reg = IODIR;
  uint8_t pin_init = 0b00000001;
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