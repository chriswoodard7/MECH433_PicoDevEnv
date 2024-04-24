#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main() {
  stdio_init_all();
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  printf("Start!\n");

  // initialize

  // initialize button pin
  gpio_init(16);
  gpio_set_dir(16, GPIO_IN);

  // initialize led pin
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // initialize adc
  adc_init();
  adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
  adc_select_input(0); // select to read from ADC0

  while (1) {
    // turn LED on
    gpio_put(LED_PIN, 1);

    // wait for button to be pressed, run rest of code
    if (gpio_get(16)) {

      // turn LED off
      gpio_put(LED_PIN, 0);

      // Asks the user to enter a number of analog samples to take, between 1 and 100
      printf("Enter number of analog samples to take (1 to 100): \r\n");

      // Reads the number entered by the user
      char message[100];
      int num;
      scanf("%s", message);
      num = atoi(message); // convert to int
      printf("number: %d\r\n", num); // confirm #

      // read adc value num times, each time at 100 Hz
      int i;

      for (i=0; i<num; i++) {
        uint16_t result = adc_read();
        printf("adc: %d\r\n", result);
        sleep_ms(10);
      }
    }
    sleep_ms(50);
  }
}