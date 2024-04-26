#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#define TABLE_SIZE 100

#define FOUR_PI (3.14159 * 4)
#define PI (3.14159)

#ifdef PICO_DEFAULT_SPI_CSN_PIN
static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

#endif

static void write_register(uint8_t channel, uint16_t data)
{
  // bit 15 is channel a (0) or b (1)
  // bit 14 is buffer control bit - default is 0
  // bit 13 is output gain selection bit (1 is 1x, 0 is 2x)
  // bit 12 is output shutdown control bit (1 active operation)
  // bit 11 to 2 - data

  // masking and bitwise operations to get 10-bit info
  uint8_t buf[2];

  uint16_t data_shift = data << 2;

  uint8_t data_byte1 = ((data_shift & 0xFF00) >> 8);
  uint8_t data_byte2 = (data_shift & 0x00FF);

  if (channel == 1){
    uint8_t control_byte = 0b10110000;
    uint8_t first_byte = (control_byte | data_byte1);
    buf[0] = first_byte;
    buf[1] = data_byte2;
  }
  if (channel == 0){
    uint8_t control_byte = 0b00110000;
    uint8_t first_byte = (control_byte | data_byte1);
    buf[0] = first_byte;
    buf[1] = data_byte2;
  }

  // write over spi
  cs_select();
  spi_write_blocking(spi_default, buf, 2);
  cs_deselect();
  sleep_ms(10);
}

int main()
{
  ////////////////////
  // INITIALIZATION //
  ////////////////////

  stdio_init_all();
  // uses SPI0 at 10kHz.
  spi_init(spi_default, 10000);
  gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

  // Chip select is active-low, so we'll initialise it to a driven-high state
  gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
  gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);


  //////////////////////////////
  // GENERATE SINE WAVE ARRAY //
  //////////////////////////////

  // 10-bit DAC - 1024 bits (0 to 1023), 3.22 mV resolution if 3.3 vref
  int samples_sine [TABLE_SIZE];
  float phaseIncrement = FOUR_PI/TABLE_SIZE;
  float currentPhase = 0.0;
  int is;

  for (is = 0; is < TABLE_SIZE; is ++)
  {
    // positive voltage from 0 to 3.3V
    // pi/2 -> 1023 = 3.3V and -pi/2 -> 0 = 0V

    samples_sine[is] = (int)floor(511.5*sin(currentPhase)+511.5);
    currentPhase += phaseIncrement;
  }

  //////////////////////////////////
  // GENERATE TRIANGLE WAVE ARRAY //
  //////////////////////////////////
  int samples_tri [TABLE_SIZE];
  int it;

  for (it = 0; it < TABLE_SIZE; it++)
  {
    // y = (A/P) * (P - abs(x % (2*P) - P) )
    samples_tri[it] = (int)floor((1023/50)*(50-abs(it % (2*50) - 50)));
    // samples_tri[it] = (int)floor((4*1023/100)*abs(((it-(100/4))%100)-50)-1023);
    // samples_tri[it] = (int)floor((2.0*1023.0/PI)*asin(sin(2*PI/100.0*it)) - 100.0);
  }


  ////////////////
  // WHILE LOOP //
  ////////////////

  while (1)
  {
    int iw;

    //for loop
    // send ith data point in samples array
    // inside do sleep_ms(10)

    // send sine wave
    // for (iw=0; iw < TABLE_SIZE; iw++){
    //   write_register(0,samples_sine[iw]);
    //   sleep_ms(10);
    // }

    // send triangle wave
    for (iw=0; iw < TABLE_SIZE; iw++){
      write_register(1,samples_tri[iw]);
      sleep_ms(10);
    }
  }
}