#include "mbed.h"
#include "codecool/codecool_shield_names.h"
#include "codecool_i2c.h"
#include "codecool_serial.h"
#include "codecool_joystick.h"
#define LM75_ADDRESS 0x90

void send_temperature_uart(){

	I2C_FREQ(100000);

	uint8_t buffer[16];
	float temp;

	buffer[0] = 0x00;
	I2C_WRITE(LM75_ADDRESS, buffer, 1);

	memset(buffer, 0x00, sizeof (buffer)); /* Remove memory trash */
	I2C_READ(LM75_ADDRESS, buffer, 2);

	int8_t _int_part = (int8_t)buffer[0];
	temp = _int_part + 0.5f * ((buffer[1]&0x80)>>7); /* save temperature as float */

	char* temp_data;
    strncpy(temp_data,(char*) temp, 4);
    SERIAL_SEND(temp_data, 4);

}

int main() {
	gpio_t led;

	gpio_init_out(&led, LED_GREEN_SHIELD);

	while(1) {
		if (JOYSTICK_LEFT == 1 ){
			get_temperature();
		}
	}
}
