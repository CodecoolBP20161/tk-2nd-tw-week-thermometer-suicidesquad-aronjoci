#include "mbed.h"
#include "codecool/codecool_shield_names.h"

void turn_off_leds(gpio_t red_led, gpio_t green_led, gpio_t blue_led){
	gpio_write(&red_led, 1);
	gpio_write(&green_led, 1);
	gpio_write(&blue_led, 1);
}

void set_led_color_based_on_temp(float temp){
	gpio_t red;
	gpio_t green;
	gpio_t blue;

	gpio_init_out(&red, LED_RED_SHIELD);
	gpio_init_out(&green, LED_GREEN_SHIELD);
	gpio_init_out(&blue, LED_BLUE_SHIELD);

}

int main() {
	while(1) {
		set_led_color_based_on_temp(1.0);
	}
}
