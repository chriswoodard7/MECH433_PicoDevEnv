#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

static void write_register()
{

}

int main()
{
  stdio_init_all();
  // uses SPI0 at 10kHz.
  spi_init(spi_default, 10 * 1000);
  gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

  // Chip select is active-low, so we'll initialise it to a driven-high state
  gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
  gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

}