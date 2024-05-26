#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "hardware/pwm.h"

#define LPWM_PIN 17
#define LDIG_PIN 16
#define RPWM_PIN 19
#define RDIG_PIN 18

void init_pins();
void set_pwm(uint8_t input_num);
void create_duty_curves(float *ld_array, float *rd_array);

static uint16_t wrap = 62500; // when to rollover, must be less than 65535
static float left_duty[101];
static float right_duty[101];

int main()
{
  // wait for usb to connect
  stdio_init_all();
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  printf("Start!\n");

  init_pins();

  // generate reference curves
  create_duty_curves(left_duty, right_duty);

  while (1){
    printf("Enter number: \r\n");

    // Reads the number entered by the user
    char message[100];
    int num;
    scanf("%s", message);
    num = atoi(message); // convert to int
    printf("number: %d\r\n", num); // confirm #
    printf("left pwm cycle #: %f\r\n", left_duty[num]);
    printf("right pwm cycle #: %f\r\n", right_duty[num]);

    set_pwm(num);
    
    sleep_ms(50);
  }
}

void init_pins(){
  ////////////////
  // LEFT MOTOR //
  ////////////////
  
  // pwm initializations
  gpio_set_function(LPWM_PIN, GPIO_FUNC_PWM); // set pin 17 to pwm
  uint slice_num1 = pwm_gpio_to_slice_num(LPWM_PIN); // Get PWM slice number

  // start at 50Hz PWM - 125MHz/62500/40 = 50
  float div = 40; // must be between 1-255
  pwm_set_clkdiv(slice_num1, div); // divider
  pwm_set_wrap(slice_num1, wrap);

  pwm_set_enabled(slice_num1, true); // turn on the PWM

  // initialize digital out pin
  gpio_init(LDIG_PIN);
  gpio_set_dir(LDIG_PIN, GPIO_OUT);



  /////////////////
  // RIGHT MOTOR //
  /////////////////
  
  // pwm initializations
  gpio_set_function(RPWM_PIN, GPIO_FUNC_PWM); // set pin 19 to pwm
  uint slice_num2 = pwm_gpio_to_slice_num(RPWM_PIN); // Get PWM slice number

  pwm_set_clkdiv(slice_num2, div); // divider
  pwm_set_wrap(slice_num2, wrap);

  pwm_set_enabled(slice_num2, true); // turn on the PWM

  // initialize digital out pin
  gpio_init(RDIG_PIN);
  gpio_set_dir(RDIG_PIN, GPIO_OUT);
}

void create_duty_curves(float *ld_array, float *rd_array){
  int i_ld = 0;
  for (i_ld=0;i_ld<100;i_ld++){
    if (i_ld==0){
      ld_array[i_ld] = 75;
    }
    if (i_ld<60){
      ld_array[i_ld] = 75-0.417*i_ld;
    }
    if (i_ld==60){
      ld_array[i_ld] = 50;
    }
    if (i_ld>60){
      ld_array[i_ld] = 125-1.25*i_ld;
    }
  }

  int i_rd = 0;
  for (i_rd=0;i_rd<100;i_rd++){
    if (i_rd==0){
      rd_array[i_rd] = 0;
    }
    if (i_rd<40){
      rd_array[i_rd] = 1.25*i_rd;
    }
    if (i_rd==40){
      rd_array[i_rd] = 50;
    }
    if (i_rd>40){
      rd_array[i_rd] = 33+0.417*i_rd;
    }
  }
}

void set_pwm(uint8_t input_num){
  // concept: 
  // if < 50, set IN2 to high and PWM duty cycle to 100% minus desired speed
  // if > 50, set IN2 to low and PWM duty cycle to desired speed
  // for now, assume that you don't need to reverse

  // set pwm of left motor
  gpio_put(LDIG_PIN, 0);
  pwm_set_gpio_level(LPWM_PIN, wrap*(left_duty[input_num]/100));
  
  // set pwm of right motor
  gpio_put(RDIG_PIN, 0);
  pwm_set_gpio_level(RPWM_PIN, wrap*(right_duty[input_num]/100));

  // 
  // if (input_num < 50){
  //   pwm_set_gpio_level(LPWM_PIN, left_duty[input_num]);
  // }
  // if (input_num == 50){

  // }
  // if (input_num > 50){

  // }
}