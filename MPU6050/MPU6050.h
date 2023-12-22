#ifndef __MPU6050_H__
#define __MPU6050_H__
#include <stdint.h>

#define MPU6050_ADDR 							0xD0
/**********     FUNCTION PROTOTYPE     **********/

/*
	* MPU6050_Wrie
	*
	* Detail			Write data to the address of the register
	* Param				Address - address of slave
								Reg			- address of register
								Data		
	* Reval				void
*/
void MPU6050_Write(uint8_t Address, uint8_t Reg, uint8_t Data);

/*
	* MPU6050_Read
	*
	* Detail			Read data to the address of the register
	* Param				Address - address of slave
								Reg			- address of register
								Buffer	- pointer of Receive data
								Size		- size of data
	* Reval				void
*/
void MPU6050_Read(uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size);

/*
	* MPU6050_Init
	*
	* Detail			Initialize MPU6050 sensor
	* Param				void
	* Reval				void
*/
void MPU6050_Init(void);

/*
	* MPU6050_ReadAccel
	*
	* Detail			Read value's Accelerometer
	*	Param				void
	* Reval				void
*/
void MPU6050_ReadAccel(void);

/*
	* MPU6050_ReadGyro
	*
	* Detail			Read value's Gyroscope
	* Param				void
	* Reval				void
*/
void MPU6050_ReadGyro(void);

#endif
