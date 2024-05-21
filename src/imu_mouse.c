#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "/pico-sdk/lib/tinyusb/hw/bsp/rp2040/board.h"
#include "/pico-sdk/lib/tinyusb/src/tusb.h"
#include "usb_descriptors.h"

// device address
#define ADDR _u(0b1101000)

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

// function prototypes
void chip_init();
void read_sensor(float *data_array);
static void send_hid_report(uint8_t report_id, float xaccel);

// hardware registers
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

  chip_init();

  while (1) {
    // use this to check if imu is working
    // uint8_t buf[2];
    // uint8_t reg = WHO_AM_I;
    // i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
    // i2c_read_blocking(i2c_default, ADDR, buf, 1, false);
    // printf("From IMU: %d\r\n", buf[0]);
    // sleep_ms(500);

    // read the data from imu sensor
    float data_array[7] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    read_sensor(data_array);

    // print data
    printf("X-Acceleration: %.2f g\r\n", data_array[0]);
    printf("Y-Acceleration: %.2f g\r\n", data_array[1]);
    printf("Z-Acceleration: %.2f g\r\n", data_array[2]);
    printf("X-Gyro Value: %.2f degrees/s\r\n", data_array[4]);
    printf("Y-Gyro Value: %.2f degrees/s\r\n", data_array[5]);
    printf("Z-Gyro Value: %.2f degrees/s\r\n", data_array[6]);
    printf("Temperature: %.2f C\r\n", data_array[3]);

    float xaccel = data_array[0];

    send_hid_report(REPORT_ID_KEYBOARD, xaccel);
    sleep_ms(100);
  }
}

void chip_init(){
  // turn on the IMU
  uint8_t on_reg[2] = {PWR_MGMT_1, 0x00};
  // uint8_t on_msg = 0x00;
  i2c_write_blocking(i2c_default, ADDR, on_reg, 2, true);  // true to keep master control of bus
  // i2c_write_blocking(i2c_default, ADDR, &on_msg, 1, false);

  // enable accel
  uint8_t on_accel[2] = {ACCEL_CONFIG, 0x00};
  // uint8_t aconfig_msg = 0x00;
  i2c_write_blocking(i2c_default, ADDR, on_accel, 2, true);  // true to keep master control of bus
  // i2c_write_blocking(i2c_default, ADDR, &aconfig_msg, 1, false);

  // enable gyro
  uint8_t on_gyro[2] = {GYRO_CONFIG, 0b00011000};
  // uint8_t gconfig_msg = 0b00011000;
  i2c_write_blocking(i2c_default, ADDR, on_accel, 2, true);  // true to keep master control of bus
  // i2c_write_blocking(i2c_default, ADDR, &gconfig_msg, 1, false);
}

void read_sensor(float *data_array){
  // bytes are axh, axl, ayh, ayl, azh, azl, th, tl, gxh, gxl, gyh, gyl, gzh, gzl
  // all of these bytes are right next to each other in memory so read 14 bytes

  uint8_t accel_reg = ACCEL_XOUT_H;
  int8_t sensor_data[14];

  i2c_write_blocking(i2c_default, ADDR, &accel_reg, 1, true);  // true to keep master control of bus
  i2c_read_blocking(i2c_default, ADDR, sensor_data, 14, false);

  // recombine accel data
  // x acceleration
  int16_t xacc_byte = ((sensor_data[0] << 8) | sensor_data[1]);
  data_array[0] = xacc_byte*0.000061;

  // y acceleration
  int16_t yacc_byte = ((sensor_data[2] << 8) | sensor_data[3]);
  data_array[1] = yacc_byte*0.000061;

  // z acceleration
  int16_t zacc_byte = ((sensor_data[4] << 8) | sensor_data[5]);
  data_array[2] = zacc_byte*0.000061;

  // recombine temp data
  int16_t temp_byte = ((sensor_data[6] << 8) | sensor_data[7]);
  data_array[3] = temp_byte/340.00 + 36.53;

  // recombine gyro data
  // x acceleration
  int16_t xgyro_byte = ((sensor_data[8] << 8) | sensor_data[9]);
  data_array[4] = xgyro_byte*0.007630;

  // y acceleration
  int16_t ygyro_byte = ((sensor_data[10] << 8) | sensor_data[11]);
  data_array[5] = ygyro_byte*0.007630;

  // z acceleration
  int16_t zgyro_byte = ((sensor_data[12] << 8) | sensor_data[13]);
  data_array[6] = zgyro_byte*0.007630;
}

static void send_hid_report(uint8_t report_id, float xaccel)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  switch(report_id)
  {
    case REPORT_ID_KEYBOARD:
    {
      // use to avoid send multiple consecutive zero report for keyboard
      static bool has_keyboard_key = false;

      if ( xaccel > 0.8 )
      {
        uint8_t keycode[6] = { 0 };
        keycode[0] = HID_KEY_A;

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
        has_keyboard_key = true;
      }else
      {
        // send empty key report if previously has key pressed
        if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
      }
    }
    break;

    // case REPORT_ID_MOUSE:
    // {
    //   int8_t const delta = 5;

    //   // no button, right + down, no scroll, no pan
    //   tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
    // }
    // break;

    // case REPORT_ID_CONSUMER_CONTROL:
    // {
    //   // use to avoid send multiple consecutive zero report
    //   static bool has_consumer_key = false;

    //   if ( btn )
    //   {
    //     // volume down
    //     uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
    //     tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
    //     has_consumer_key = true;
    //   }else
    //   {
    //     // send empty key report (release key) if previously has key pressed
    //     uint16_t empty_key = 0;
    //     if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
    //     has_consumer_key = false;
    //   }
    // }
    // break;

    // case REPORT_ID_GAMEPAD:
    // {
    //   // use to avoid send multiple consecutive zero report for keyboard
    //   static bool has_gamepad_key = false;

    //   hid_gamepad_report_t report =
    //   {
    //     .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
    //     .hat = 0, .buttons = 0
    //   };

    //   if ( btn )
    //   {
    //     report.hat = GAMEPAD_HAT_UP;
    //     report.buttons = GAMEPAD_BUTTON_A;
    //     tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

    //     has_gamepad_key = true;
    //   }else
    //   {
    //     report.hat = GAMEPAD_HAT_CENTERED;
    //     report.buttons = 0;
    //     if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
    //     has_gamepad_key = false;
    //   }
    // }
    // break;

    default: break;
  }
}