#include "mbed.h"
#include "codecool/codecool_shield_names.h"
#include "codecool/codecool_lcd.h"
#include "codecool_i2c.h"
#include "codecool_serial.h"
#include "codecool_joystick.h"
#define LM75_ADDRESS 0x90

void send_temperature_uart(){
    I2C_FREQ(100000);
    SERIAL_BAUD(9600);
    SERIAL_SET_NON_BLOCKING();

    uint8_t buffer[16];
    buffer[0] = 0x00;
    I2C_WRITE(LM75_ADDRESS, buffer, 1);

    memset(buffer, 0x00, sizeof (buffer));
    I2C_READ(LM75_ADDRESS, buffer, 2);


    buffer[0] = (int8_t)buffer[0];
    buffer[1] = (int8_t)(0.5f * ((buffer[1]&0x80)>>7));

    LCD_CLS();
    LCD_LOCATE(0, 0);
    LCD_PRINTF("Temperature: %02X %02X",
    		buffer[0], buffer[1]);

}

float get_temperature_uart(){
    SERIAL_BAUD(9600);
    SERIAL_SET_NON_BLOCKING();
    uint8_t buffer[16];
    float temp;
    temp = (float)buffer[0] + (float)buffer[1]/10;
    return temp;
}

/*Turn off the red, green and blue component of the RGB LED,
 *because after initializing the RGB led components, they are turned in
 *by default*/
void turn_off_leds(gpio_t red_led, gpio_t green_led, gpio_t blue_led){
	gpio_write(&red_led, 1);
	gpio_write(&green_led, 1);
	gpio_write(&blue_led, 1);
}


void set_led_color_based_on_temp(float temp){
	/*Create gpio variables.*/
	gpio_t red;
	gpio_t green;
	gpio_t blue;
	LCD_CLS();

	/*Initialize pins, which are controlling the RGB led components,
	 * as outputs.*/
	gpio_init_out(&red, LED_RED_SHIELD);
	gpio_init_out(&green, LED_GREEN_SHIELD);
	gpio_init_out(&blue, LED_BLUE_SHIELD);

	/*The main logic, which turns on specific led components,
	 * based on the given temperature data.*/
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
		if (JOYSTICK_LEFT == 1 ){
			send_temperature_uart();
			while(SERIAL_AVAILABLE() < 2 );
			wait(0.2);
			LCD_CLS();
		}
		if (JOYSTICK_RIGHT == 1 ){
			float temp = get_temperature_uart();
			set_led_color_based_on_temp(temp);
			LCD_LOCATE(10,10);
			LCD_PRINTF("%f", temp);
			wait(0.2);
			LCD_CLS();
		}
		wait(1);
	}
}
