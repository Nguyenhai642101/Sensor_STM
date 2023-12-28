#include <stdio.h>
#include <stdint.h>
#include "BMP280.h"
#include "I2C.h"

double Temp, Press;
int main(void)
{
	Timer_Init();
	I2C_config();
	BMP280_Init();
	while(1)
	{
		Temp = BMP280_GetTemp();
		Press = BMP280_GetPress();
		delay_ms(1000);
	}
}