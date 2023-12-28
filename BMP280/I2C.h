#ifndef __I2C_LIB_H__
#define __I2C_LIB_H__
#include <stdint.h>
#include <stdio.h>


void I2C_config(void);
void I2C_Start(void);
void I2C_Write(uint8_t data);
void I2C_SendAdd(uint8_t address);
void I2C_Stop(void);
void I2C_WriteMulti(uint8_t *data, uint8_t size);
void I2C_Read(uint8_t Address, uint8_t *buffer, uint8_t size);


#endif