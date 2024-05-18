#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

// device address
#define ADDR _u(0b1101000)

// hardware registers
int main()
{
  stdio_init_all();
  while (!stdio_usb_connected()){
    sleep_ms(100);
  }
  printf("Start!\r\n");

  i2c_init(i2c_default, 100 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  // gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  // gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  while (1) {
    uint8_t buf = 0;
    i2c_read_blocking(i2c_default, ADDR, &buf, 1, false);
    printf("From IMU: %x\r\n", buf);
    sleep_ms(500);
  }
}

void chip_init() {
    // // use the "handheld device dynamic" optimal setting (see datasheet)
    // uint8_t buf[2];

    // // 500ms sampling time, x16 filter
    // const uint8_t reg_config_val = ((0x04 << 5) | (0x05 << 2)) & 0xFC;

    // // send register number followed by its corresponding value
    // buf[0] = REG_CONFIG;
    // buf[1] = reg_config_val;
    // i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

    // // osrs_t x1, osrs_p x4, normal mode operation
    // const uint8_t reg_ctrl_meas_val = (0x01 << 5) | (0x03 << 2) | (0x03);
    // buf[0] = REG_CTRL_MEAS;
    // buf[1] = reg_ctrl_meas_val;
    // i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

}

// i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
// i2c_read_blocking(i2c_default, ADDR, buf, 6, false);  // false - finished with bus