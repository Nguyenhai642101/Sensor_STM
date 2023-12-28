#include "BMP280.h"

uint16_t dig_T1,  dig_P1;	
int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6,
				dig_P7, dig_P8, dig_P9;
int32_t t_fine;
int32_t adc_T;
uint8_t RawTemp[3] = {0};
int32_t adc_P;
uint8_t RawPress[3] = {0};

/*
	* BMP280_Wrtie
	*
	* Detail			Send address of I2C slave and write data to Reg register
	* Param				Address : address of I2C
								Reg			: address of register that want to write data
								Data		
	* Reval				void
*/
void BMP280_Write(uint8_t Address, uint8_t Reg, uint8_t Data)
{
	// format of frame in the Figure 7, page 28 in the document
	I2C_Start();
	I2C_SendAdd(Address);
	I2C_Write(Reg);
	I2C_Write(Data);
	I2C_Stop();
}

/*
	* BMP280_Read
	* 
	* Detail			Read data from Reg register and transmitter to buffer
	* Param				Address : address of I2C
								Reg			: address of register that want to read data
								buffer	: pointer that store data from Reg register
								size		: the number of bit need to get from Reg register
	* Reval				void
*/
void BMP280_Read(uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size)
{
	// format of frame in the Figure 8, page 29 in the document
	I2C_Start();
	I2C_SendAdd(Address);
	I2C_Write(Reg);
	I2C_Start();
	I2C_Read(Address + 0x01, buffer, size);
	I2C_Stop();
}

/*
	* BMP280_ReadCallibData
	* 
	* Detail			Read data's Callibration of sensor from address 0x88 -> 0xA1
	* Param				void
	* Reval				void
*/
void BMP280_ReadCallibData(void)
{
	uint8_t Callib_Data[23] = {0};
	BMP280_Read(BMP280_ADDR, BMP280_CALLIB00, Callib_Data, 23);
	
	dig_T1 = ((Callib_Data[0] << 8) | Callib_Data[1]);
	dig_T2 = ((Callib_Data[2] << 8) | Callib_Data[3]);
	dig_T3 = ((Callib_Data[4] << 8) | Callib_Data[5]);
	dig_P1 = ((Callib_Data[6] << 8) | Callib_Data[7]);
	dig_P2 = ((Callib_Data[8] << 8) | Callib_Data[7]);
	dig_P3 = ((Callib_Data[10] << 8) | Callib_Data[9]);
	dig_P4 = ((Callib_Data[12] << 8) | Callib_Data[11]);
	dig_P5 = ((Callib_Data[14] << 8) | Callib_Data[13]);
	dig_P6 = ((Callib_Data[16] << 8) | Callib_Data[15]);
	dig_P7 = ((Callib_Data[18] << 8) | Callib_Data[17]);
	dig_P8 = ((Callib_Data[20] << 8) | Callib_Data[19]);
	dig_P9 = ((Callib_Data[22] << 8) | Callib_Data[21]);
}

/*
	* BMP280_Init
	*
	* Detail			Initialize BMP280 sensor
	* Param				void
	* Reval				void
*/
void BMP280_Init(void)
{
	uint8_t check;
	uint8_t data;
	// check ID of chip
	BMP280_Read(BMP280_ADDR, BMP280_ID, &check, 1);
	if(check == 88) // 0x58 will be returned by the sensor if everything goes well
	{
		// Reset sensor
		data = 0xB6;
		BMP280_Write(BMP280_ADDR, BMP280_RESET, data);
		
		// set status od the device when complete conversion
		data = 0x09;
		BMP280_Write(BMP280_ADDR, BMP280_STATUS, data);
		
		// choose mode, controls oversampling of pressure data, controls oversampling of temperature data
		data = 0x2B;
		BMP280_Write(BMP280_ADDR, BMP280_CTRL_MEAS, data);
		
		// configure the rate,, filter and interface options of the device
		data = 0x04;
		BMP280_Write(BMP280_ADDR, BMP280_CONFIG, data);
	}
}

/*
	* BMP280_GetRawTemp
	*
	* Detail			Get 20bits raw data from address: 0xFA, 0xFB, 0xFC (0xFA/0xFB/0xFC[7:4])
	* Param				void
	* Reval				int32_t	
*/
int32_t BMP280_GetRawTemp(void)
{
	//uint8_t RawTemp[3] = {0};
	BMP280_Read(BMP280_ADDR, BMP280_TEMP_MSB, RawTemp, 3);
	return (int32_t)((RawTemp[0] << 12) | (RawTemp[1] << 4) | (RawTemp[2] >> 4)); 
}

/*
	* BMP280_GetTemp
	* 
	* Detail			Calculate the temperature value based on the formula provided by the datasheet
	* Param				void
	* Reval				double
*/
double BMP280_GetTemp(void)
{
	//int32_t adc_T = BMP280_GetRawTemp();
	BMP280_ReadCallibData();
	adc_T = BMP280_GetRawTemp();
	double var1, var2, T;
	var1 = (((double) adc_T) / 16384.0 - ((double) dig_T1) / 1024.0) * ((double) dig_T2);
	var2 = ((((double) adc_T) / 131072.0 - ((double) dig_T1) / 8192.0) * (((double) adc_T) / 131072.0 - ((double) dig_T1) / 8192.0)) * ((double) dig_T3);
	t_fine = (int32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
//	var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
//	var2 = (((((adc_T >> 4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3)) >> 14;
//	t_fine = var1 + var2;
//	T = (t_fine * 5 + 128) >> 8;
	return T;
}

/*
	* BMP280_GetRawPress
	*
	* Detail			Get 20bits raw data from address: 0xF7, 0xF8, 0xF9 (0xF7/0xF8/0xF9[7:4])
	* Param				void
	* Reval				int32_t	
*/
int32_t BMP280_GetRawPress(void)
{
	//uint8_t RawPress[3] = {0};
	BMP280_Read(BMP280_ADDR, BMP280_PRESS_MSB, RawPress, 3);
	return (int32_t)((RawPress[0] << 12) | (RawPress[1] << 4) | (RawPress[2] >> 4));
}

/*
	* BMP280_GetPress
	* 
	* Detail			Calculate the pressure value based on the formula provided by the datasheet
	* Param				void
	* Reval				double
*/
double BMP280_GetPress(void)
{
	//int32_t adc_P = BMP280_GetRawPress();
	BMP280_ReadCallibData();
	adc_P = BMP280_GetRawPress();
	double var1, var2, p;
	var1 = ((double) t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double) dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double) dig_P5) * 2.0;
	var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
	var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double) dig_P1);
	if(var1 == 0)
	{
		return 0;
	}
	p = 1048576.0 - (double) adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double) dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double) dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double) dig_P7)) / 16.0;
//	var2 = var2 + ((var1 * ((int32_t) dig_P5)) << 1);
//	var2 = (var2 >> 2) + (((int32_t) dig_P4) << 16);
//	var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t) dig_P2) * var1) >> 1)) >> 18;
//	var1 = ((((var1 + 32768)) * (int32_t) dig_P1)) >> 15;
//	if(var1 == 0)
//	{
//		return 0;		// avoid exception caused by division by zero
//	}
//	p = (((uint32_t)(((int32_t) 1048576) - adc_P) - (var2 >> 12))) *  3125;
//	if(p < 0x80000000)
//	{
//		p = (p << 1) / ((uint32_t) var1);
//	}
//	else
//	{
//		p = (p / (uint32_t) var1) * 2;
//	}
//	var1 = (((int32_t) dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
//	var2 = (((int32_t)(p >> 2)) * ((int32_t) dig_P8)) >> 13;
//	p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}