#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWMPin 15

void init_servo();
void set_angle();

int main()
{
    init_servo();
    while (1){
        set_angle();
        sleep_ms(2000);
    }
}

void init_servo(){
    // INITIALIZE THE SERVO
    gpio_set_function(PWMPin, GPIO_FUNC_PWM); // Set pin 29 to be PWM
    uint slice_num = pwm_gpio_to_slice_num(PWMPin); // Get PWM slice number

    // start at 50Hz PWM - 125MHz/62500/40 = 50
    float div = 40; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 62500; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true); // turn on the PWM
}

void set_angle(){
    uint16_t wrap = 62500;
    pwm_set_gpio_level(PWMPin, wrap / 40); // set to 0 degrees
    sleep_ms(2000);
    pwm_set_gpio_level(PWMPin, wrap / 8); // set to 180 degrees
    // sleep_ms(2000);
    // pwm_set_gpio_level(PWMPin, wrap / 40); // set to 0 degrees
}