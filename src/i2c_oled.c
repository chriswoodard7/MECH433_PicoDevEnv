#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "hardware/adc.h"

unsigned char SSD1306_ADDRESS = 0b0111100; // 7bit i2c address
unsigned char ssd1306_buffer[513]; // 128x32/8. Every bit is a pixel except first byte
volatile unsigned int t_elapse = 0;

void ssd1306_setup();
void ssd1306_command(unsigned char c);
void ssd1306_update();
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color);
void ssd1306_clear();

int main()
{
  stdio_init_all();
  // while (!stdio_usb_connected()){
  //   sleep_ms(100);
  // }
  // printf("Start!\r\n");

  // initialize adc
  adc_init();
  adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
  adc_select_input(0); // select to read from ADC0

  // I2C Initializations
  i2c_init(i2c_default, 100 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

  // initialize OLED screen
  ssd1306_setup();

  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (1){

    gpio_put(LED_PIN, 1);

    // get ADC value
    uint16_t result = adc_read();
    float result_f = (float)result/4095.0*3.3;
    char message[50];
    sprintf(message, "ADC: %.3f (V)", result_f);

    // display ADC value  
    unsigned int t_start = to_us_since_boot(get_absolute_time());
    drawString(40,5,message);

    // display frame rate
    if (t_elapse != 0){
      sprintf(message, "FPS: %.2f", (float)(1/(t_elapse*0.000001)));
      drawString(40,23,message);  
    }
    ssd1306_update();

    // turn off LED and pixel
    gpio_put(LED_PIN, 0);
    ssd1306_clear();
    ssd1306_update();

    unsigned int t_end = to_us_since_boot(get_absolute_time());

    // calculate elapsed time to print, update, and clear screen
    t_elapse = t_end - t_start;

    sleep_ms(100);
  }
}

void ssd1306_setup() {
    // first byte in ssd1306_buffer is a command
    ssd1306_buffer[0] = 0x40;
    // give a little delay for the ssd1306 to power up
    //_CP0_SET_COUNT(0);
    //while (_CP0_GET_COUNT() < 48000000 / 2 / 50) {
    //}
    sleep_ms(20);
    ssd1306_command(SSD1306_DISPLAYOFF);
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_command(0x80);
    ssd1306_command(SSD1306_SETMULTIPLEX);
    ssd1306_command(0x1F); // height-1 = 31
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_command(0x0);
    ssd1306_command(SSD1306_SETSTARTLINE);
    ssd1306_command(SSD1306_CHARGEPUMP);
    ssd1306_command(0x14);
    ssd1306_command(SSD1306_MEMORYMODE);
    ssd1306_command(0x00);
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYON);
    ssd1306_clear();
    ssd1306_update();
}

// send a command instruction (not pixel data)
void ssd1306_command(unsigned char c) {
    //i2c_master_start();
    //i2c_master_send(ssd1306_write);
    //i2c_master_send(0x00); // bit 7 is 0 for Co bit (data bytes only), bit 6 is 0 for DC (data is a command))
    //i2c_master_send(c);
    //i2c_master_stop();

    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] =c;
    i2c_write_blocking(i2c_default, SSD1306_ADDRESS, buf, 2, false);
}

// update every pixel on the screen
void ssd1306_update() {
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0);
    ssd1306_command(0xFF);
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(128 - 1); // Width

    unsigned short count = 512; // WIDTH * ((HEIGHT + 7) / 8)
    unsigned char * ptr = ssd1306_buffer; // first address of the pixel buffer
    /*
    i2c_master_start();
    i2c_master_send(ssd1306_write);
    i2c_master_send(0x40); // send pixel data
    // send every pixel
    while (count--) {
        i2c_master_send(*ptr++);
    }
    i2c_master_stop();
    */

    i2c_write_blocking(i2c_default, SSD1306_ADDRESS, ptr, 513, false);
}

// set a pixel value. Call update() to push to the display)
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color) {
    if ((x < 0) || (x >= 128) || (y < 0) || (y >= 32)) {
        return;
    }

    if (color == 1) {
        ssd1306_buffer[1 + x + (y / 8)*128] |= (1 << (y & 7));
    } else {
        ssd1306_buffer[1 + x + (y / 8)*128] &= ~(1 << (y & 7));
    }
}

// zero every pixel value
void ssd1306_clear() {
    memset(ssd1306_buffer, 0, 512); // make every bit a 0, memset in string.h
    ssd1306_buffer[0] = 0x40; // first byte is part of command
}

void drawChar(unsigned char x, unsigned char y, char c){
  
  // loop through the width of the letter
  int i;
  for (i=0;i<5;i++){

    // store the y pixels of the letter at this x position
    int ascii_height = ASCII[c][i];
    // printf("%d\r\n", ascii_height);

    // loop through the y pixels at the ith x pixel
    int ii = 0;
    while (ii<8){
      int last_bit = (ascii_height & 0b00000001);
      // printf("%d\r\n", last_bit);

      // draw pixel if the bit is a 1
      if (last_bit == 1){
        ssd1306_drawPixel(x+i, y+ii, 1);
      }
      ii++;
      ascii_height = ascii_height >> 1;
    }
  }
}

void drawString(unsigned char x, unsigned char y, char *s){
  int string_len = strlen(s);
  int i = 0;

  for (i=0;i<string_len;i++){
    int start_x = i*5;
    int dec_char = s[i]-32;
    drawChar(start_x, y, dec_char);
  }
}