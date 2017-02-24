#include "mbed.h"
#include "codecool/codecool_shield_names.h"
#include "codecool/codecool_lcd.h"
#include "codecool_i2c.h"
#include "codecool_pwm.h"
#include "codecool_serial.h"
#include "codecool_joystick.h"
#include "codecool_pot.h"
#define LM75_ADDRESS 0x90


void send_temperature_uart(){
    uint8_t buffer[16];
    /*Initializations to temperature sensor and UART communication*/
    I2C_FREQ(100000);
    SERIAL_BAUD(9600);
    SERIAL_SET_NON_BLOCKING();

    /*Set buffer to zero and get temperature data*/
    memset(buffer, 0x00, sizeof (buffer));
    I2C_WRITE(LM75_ADDRESS, buffer, 1);
    memset(buffer, 0x00, sizeof (buffer));
    I2C_READ(LM75_ADDRESS, buffer, 2);

    /*Print the sent temperature data*/
    LCD_CLS();
    LCD_LOCATE(0, 0);
    LCD_PRINTF("Sending:   %d %d",
    		 buffer[0], buffer[1]);

    /*Manipulate the temperature data on the sender side by +-10 based on POT1 value*/
    if(POT1_READ<0.1){
    	buffer[0]-=10;
    }
    if(POT1_READ>0.9){
    	buffer[0]+=10;
    }

    /*Send the temperature data over UART*/
    SERIAL_SEND(buffer, 16);

}

/*Based on POT1 value it returns the distance of the scale limits from
 * the middle value*/
float set_temp_scale_sensitivity(){
	if(POT1_READ<=0.33){
		return 2.0;
	}
	else if(0.33<POT1_READ && POT1_READ<=0.66){
		return 4.0;
	}
	else{
		return 6.0;
	}
}

float get_temperature_uart(){
    float tempr;
    uint8_t buffer[16];

    /*Initializations to temperature sensor and UART communication*/
    SERIAL_BAUD(9600);
    SERIAL_SET_NON_BLOCKING();

    /*Set buffer to zero and receive the temperature data over UART*/
    memset(buffer, 0x00, sizeof(buffer));
    SERIAL_RECV(buffer, 16);

    /*Transform th temperature data to the necessary form.*/
    int8_t _int_part = (int8_t)buffer[0];
    tempr = _int_part + 0.5f * ((buffer[1]&0x80)>>7);

    /*Print the received temerature data.*/
    LCD_LOCATE(0,12);
    LCD_PRINTF("Received:   %.1f", tempr);
    wait(1);
    return tempr;
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
	float middle_value = 22.0;
	float scale_modifier = set_temp_scale_sensitivity();

	/*The main logic, which turns on specific led components,
	 * based on the given temperature data.*/
	if(temp<middle_value-3*scale_modifier){
		turn_off_leds(red, green, blue);
		gpio_write(&blue, 0);
	}
	else if(middle_value-3*scale_modifier<=temp &&
			temp<middle_value-scale_modifier){
		turn_off_leds(red, green, blue);
		gpio_write(&green, 0);
		gpio_write(&blue, 0);
	}
	else if(middle_value-scale_modifier<=temp &&
			temp<middle_value+scale_modifier){
		turn_off_leds(red, green, blue);
		gpio_write(&green, 0);
	}
	else if(middle_value+scale_modifier<=temp && temp<middle_value+3*scale_modifier){
		turn_off_leds(red, green, blue);
		gpio_write(&red, 0);
		gpio_write(&green, 0);
	}
	else if(middle_value+3*scale_modifier<=temp){
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
		/*Print the instructions on the LCD, a out how can you put a device to
		 * sender and to receiver state*/
		if (JOYSTICK_LEFT == 0 && JOYSTICK_RIGHT == 0){
			LCD_CLS();
			LCD_LOCATE(0,0);
			LCD_PRINTF("<-- Send temperature \n Receive temperature -->");
			wait(1);
		}
		/*Send temperature data over UART*/
		if (JOYSTICK_LEFT == 1 ){
			LCD_CLS();
			send_temperature_uart();
			wait(1);
		}
		/*Get temperature data over UART*/
		if (JOYSTICK_RIGHT == 1 ){
			LCD_CLS();
			float temp = get_temperature_uart();
			set_led_color_based_on_temp(temp);
		}
		wait(1);
	}
}
