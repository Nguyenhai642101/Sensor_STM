#ifndef __BMP280_H__
#define __BMP280_H__

#include <stdint.h>
#include "I2C.h"
#include "BMP280_Register.h"

/*
	* BMP280_ReadCallibData
	* 
	* Detail			Read data's Callibration of sensor from address 0x88 -> 0xA1
	* Param				void
	* Reval				void
*/
void BMP280_ReadCallibData(void);

/*
	* BMP280_GetTemp
	* 
	* Detail			Calculate the temperature value based on the formula provided by the datasheet
	* Param				void
	* Reval				double
*/
double BMP280_GetTemp(void);

/*
	* BMP280_GetPress
	* 
	* Detail			Calculate the pressure value based on the formula provided by the datasheet
	* Param				void
	* Reval				double
*/
double BMP280_GetPress(void);

/*
	* BMP280_Init
	*
	* Detail			Initialize BMP280 sensor
	* Param				void
	* Reval				void
*/
void BMP280_Init(void);

#endif
