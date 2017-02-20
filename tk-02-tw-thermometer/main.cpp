#include "mbed.h"
#include "codecool/codecool_shield_names.h"
#include "codecool/codecool_lcd.h"

void turn_off_leds(gpio_t red_led, gpio_t green_led, gpio_t blue_led){
	gpio_write(&red_led, 1);
	gpio_write(&green_led, 1);
	gpio_write(&blue_led, 1);
}

void set_led_color_based_on_temp(float temp){
	gpio_t red;
	gpio_t green;
	gpio_t blue;
	LCD_CLS();

	gpio_init_out(&red, LED_RED_SHIELD);
	gpio_init_out(&green, LED_GREEN_SHIELD);
	gpio_init_out(&blue, LED_BLUE_SHIELD);

	if(temp<10.0){
		turn_off_leds(red, green, blue);
		gpio_write(&blue, 0);
	}
	else if(10.0<=temp && temp<18.0){
		turn_off_leds(red, green, blue);
		gpio_write(&green, 0);
		gpio_write(&blue, 0);
	}
	else if(18.0<=temp && temp<26.0){
		turn_off_leds(red, green, blue);
		gpio_write(&green, 0);
	}
	else if(26.0<=temp && temp<34.0){
		turn_off_leds(red, green, blue);
		gpio_write(&red, 0);
		gpio_write(&green, 0);
	}
	else if(34.0<=temp){
		turn_off_leds(red, green, blue);
		gpio_write(&red, 0);
	}
	else{
		LCD_LOCATE(60, 15);
		LCD_PRINTF("Not valid temperature data given!");
	}
}

int main() {
	while(1) {
		set_led_color_based_on_temp(1.0);
	}
}
