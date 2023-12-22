#include "MPU6050.h"
#include "MPU6050_Register.h"
#include "Timer.h"


uint16_t Accel_X_RAW = 0;
uint16_t Accel_Y_RAW = 0;
uint16_t Accel_Z_RAW = 0;

uint16_t Gyro_X_RAW = 0;
uint16_t Gyro_Y_RAW = 0;
uint16_t Gyro_Z_RAW = 0;

uint8_t check;

float Ax, Ay, Az, Gx, Gy, Gz;

/*
	* MPU6050_Wrie
	*
	* Detail			Write data to the address of the register
	* Param				Address - address of slave
								Reg			- address of register
								Data		
	* Reval				void
*/
void MPU_Write(uint8_t Address, uint8_t Reg, uint8_t Data)
{
	/**** STEPS FOLLOWED  ************
	1. START the I2C
	2. Send the ADDRESS of the Device
	3. Send the ADDRESS of the Register, where you want to write the data to
	4. Send the DATA
	5. STOP the I2C
	*/
	
	I2C_Start();
	I2C_SendAdd(Address);
	I2C_Write(Reg);
	I2C_Write(Data);
	I2C_Stop();
}

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
void MPU_Read(uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size)
{
	/**** STEPS FOLLOWED  ************
	1. START the I2C
	2. Send the ADDRESS of the Device
	3. Send the ADDRESS of the Register, where you want to READ the data from
	4. Send the RESTART condition
	5. Send the Address (READ) of the device
	6. Read the data
	7. STOP the I2C
	*/
	I2C_Start();
	I2C_SendAdd(Address);
	I2C_Write(Reg);
	I2C_Start();
	I2C_Read(Address+0x01, buffer, size);
	I2C_Stop();
}

/*
	* MPU6050_Init
	*
	* Detail			Initialize MPU6050 sensor
	* Param				void
	* Reval				void
*/
void MPU6050_Init(void)
{
	uint8_t check;
	uint8_t data;
	// check ID WHO_AM_I
	MPU_Read(MPU6050_ADDR, WHO_AM_I, &check, 1);
	if(check == 0x68) // 0x68 will be returned by the sensor if eveything goes well
	{
		// power management register 0x6B we should write all 0's to wake the sensor up
		data = 0;
		MPU_Write(MPU6050_ADDR, PWR_MGMT_1, data);
		
		// set Data rate of 1 Khz by writting SMPLRT_DIV register
		data = 0x07;
		MPU_Write(MPU6050_ADDR, SMPLRT_DIV, data);
		
		// set accelerometer configuration in ACCEL_CONFIG register
		// XA_ST = 0, YA_ST = 0, ZA_ST = 0, FS_SEL = 0 -> ? 2g
		data = 0x00;
		MPU_Write(MPU6050_ADDR, ACCEL_CONFIG, data);
		
		// set Gyroscopic configuration in GYRO_CONFIG register
		// XG_ST = 0, YG_ST = 0, ZG_ST = 0, FS_SEL = 0 -> ? 250 ?/s
		data = 0x00;
		MPU_Write(MPU6050_ADDR, GYRO_CONFIG, data);
	}
}

/*
	* MPU6050_ReadAccel
	*
	* Detail			Read value's Accelerometer
	*	Param				void
	* Reval				void
*/
void MPU6050_ReadAccel(void)
{
	uint8_t Rx_data[6];
	
	// read 6 byte of data starting from ACCEL_XOUT_H register
	MPU_Read(MPU6050_ADDR, ACCEL_XOUT_H, Rx_data, 6);
	
	Accel_X_RAW = (uint16_t) (Rx_data[0] << 8 | Rx_data[1]); 
	Accel_Y_RAW = (uint16_t) (Rx_data[2] << 8 | Rx_data[3]); 
	Accel_Z_RAW = (uint16_t) (Rx_data[4] << 8 | Rx_data[5]); 
	
	/* 
		Convert the RAW values into acceleration in 'g'
		we have to divide according to the Full scale value set in FS_SEL
		I have configured FS_SEL = 0. So I am dividing by 16384
		for more details check ACCEL_CONFIG register.
	*/
	Ax = Accel_X_RAW / 16384.0;
	Ay = Accel_Y_RAW / 16384.0;
	Az = Accel_Z_RAW / 16384.0;
}

/*
	* MPU6050_ReadGyro
	*
	* Detail			Read value's Gyroscope
	* Param				void
	* Reval				void
*/
void MPU6050_ReadGyro(void)
{
	uint8_t Rx_data[6];
	MPU_Read(MPU6050_ADDR, GYRO_XOUT_H, Rx_data, 6);
	
	Gyro_X_RAW = (uint16_t)(Rx_data[0]<<8 | Rx_data[1]);
	Gyro_Y_RAW = (uint16_t)(Rx_data[2]<<8 | Rx_data[3]);
	Gyro_Z_RAW = (uint16_t)(Rx_data[4]<<8 | Rx_data[5]);
	
	Gx = Gyro_X_RAW / 131.0;
	Gy = Gyro_Y_RAW / 131.0;
	Gz = Gyro_Z_RAW / 131.0;
}