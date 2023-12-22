#include "i2c.h"
#include "stm32f10x.h"

void I2C_config(void)
{
	RCC->APB1ENR |= 1<<21;									// enable i2c1
	RCC->APB2ENR |= 1<<3 | 1<<0;						// enable GPIOB
	
	GPIOB->CRH = 0; 												// clear CRH register
	GPIOB->CRH |= 3<<2;											// alternate function output open-drain (B8)
	GPIOB->CRH |= 3<<0;											// output mode, 50Mhz
	GPIOB->CRH |= 3<<6;											// alternate function output open-drain (B9)
	GPIOB->CRH |= 3<<4;											// output mode, 50Mhz
	AFIO->MAPR |= 1<<1;											// select PB8: SCL, PB9: SDA (if remap = 0: PB6: SCL, PB7: SDA)
	
	/*  Configure I2C  */
	I2C1->CR1 = 0;													// clear CR1 register
	I2C1->CR1 |= 1<<15;    									// select Peripheral under reset satte
	I2C1->CR1 &= ~(1<<15);									// When set, the I2C is under reset state. Before resetting this bit, 
																					// make sure the I2C lines are released and the bus is free.
	I2C1->CR2 = 0;													// clear CR2 bit
	I2C1->CR2 |= 36<<0;											// set Peripheral clock frequency: 36Mhz
	I2C1->CCR |= 180<<0;										// set clock control register CCR = 5000/(T) with T = 1/(Peripheral clock frequency)
																					// ex:  Peripheral clock frequency = 366Mhz => T = 27,778(ns) => CCR = 5000/27,778 = 180
	I2C1->TRISE = 37;												// TRISE = (Tr(SCL) / Tpclk1) + 1
																					// ex: TRISE = 1000/27,778 + 1 => TRISE = 37
	/*   Enable I2C   */
	I2C1->CR1 |= 1<<0;											// enable Peripheral with bit 0
	
}

void I2C_Start(void)
{
	/**** 	START I2C  ************
	1. Enable the ACK
	2. Send the START condition 
	3. Wait for the SB ( Bit 0 in SR1) to set. This indicates that the start condition is generated
	*/	
	
	I2C1->CR1 |= 1<<10;											// enable ACK bit
	I2C1->CR1 |= 1<<8;											// Start generation
	while(!(I2C1->SR1 & (1<<0)));
}

void I2C_Write(uint8_t data)
{
	/**** STOP I2C  ************
	1. Wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty
	2. Send the DATA to the DR Register
	3. Wait for the BTF (bit 2 in SR1) to set. This indicates the end of LAST DATA transmission
	*/
	while(!(I2C1->SR1 & (1<<7)));
	I2C1->DR = data;
	while(!(I2C1->SR1 & (1<<2)));
	
}

void I2C_SendAdd(uint8_t address)
{
	/****  SEND ADDRESS  ************
	1. Send the Slave Address to the DR Register
	2. Wait for the ADDR (bit 1 in SR1) to set. This indicates the end of address transmission
	3. clear the ADDR by reading the SR1 and SR2
	*/	
	I2C1->DR = address;
	while(!(I2C1->SR1 & (1<<1)));
	uint8_t temp = I2C1->SR1 | I2C1->SR2;			// read SR1 and SR2 to clear the ADDR bit
}

void I2C_Stop(void)
{
	I2C1->CR1 |= 1<<3;
}

void I2C_WriteMulti(uint8_t *data, uint8_t size)
{
	/*			WRITE MULTI
	1. Wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty
	2. Keep sending Data to the DR register after performing the check if the TXE bit is set
	3. Once the Data transfer is complete, wait for the BTF (bit 2 in SR1) to set. This indicates the end of last data transmission
	*/
	while(!(I2C1->SR1 & (1<<7)));
	while(size)
	{
		while(!(I2C1->SR1 & (1<<7)));
		I2C1->DR = (volatile uint32_t) *data++;		// send data
		size--;
	}
	while(!(I2C1->SR1 & (1<<2)));
}

void I2C_Read(uint8_t Address, uint8_t *buffer, uint8_t size)
{
	/**** STEPS FOLLOWED  ************
	1. If only 1 BYTE needs to be Read
		a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
		b) the Acknowledge disable is made during EV6 (before ADDR flag is cleared) and the STOP condition generation is made after EV6
		c) Wait for the RXNE (Receive Buffer not Empty) bit to set
		d) Read the data from the DR

	2. If Multiple BYTES needs to be read
		a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
		b) Clear the ADDR bit by reading the SR1 and SR2 Registers
		c) Wait for the RXNE (Receive buffer not empty) bit to set
		d) Read the data from the DR 
		e) Generate the Acknowlegment by settint the ACK (bit 10 in SR1)
		f) To generate the nonacknowledge pulse after the last received data byte, the ACK bit must be cleared just after reading the 
			 second last data byte (after second last RxNE event)
		g) In order to generate the Stop/Restart condition, software must set the STOP/START bit 
			 after reading the second last data byte (after the second last RxNE event)
	*/
	uint8_t remaining = size;
	if(size == 1)
	{
		// step 1-a
		I2C1->DR = Address;													// send the address
		while(!(I2C1->SR1 & (1<<1)));								// wait for ADDR bit to set
		// step 1-b
		I2C1->CR1 &= ~(1<<10);											// clear the ACK bit
		uint8_t temp = I2C1->SR1 | I2C1->SR2;				// read SR1 and SR2 to clear the ADDR bit
		I2C1->CR1 |= (1<<9);												// stop I2C
		// step 1-c
		while(!(I2C1->SR1 & (1<<6)));								// wait for RxNE to set
		// step 1-d
		buffer[size - remaining] = I2C1->DR;				// read the data from the DR
	}
	else{
		// step 2-a
		I2C1->DR = Address;													// send the address
		while(!(I2C1->SR1 & (1<<1)));								// wait for ADDR bit to set
		// step 2-b
		uint8_t temp = I2C1->SR1 | I2C1->SR2;				// read SR1 and SR2 to clear the ADDR bit
		
		while(remaining > 2)
		{
			// step 2-c
			while(!(I2C1->SR1 & (1<<6)));							// wait for RxNE to set
			// step 2-d
			buffer[size - remaining] = I2C1->DR;			// copy the data into the buffer
			// step 2-e
			I2C1->CR1 |= 1<<10;												// set the ACK bit to Acknowledge the data received
			
			remaining--;
		}
		
		// read the second last byte
		while(!(I2C1->SR1 & (1<<6)));								// wait for RxNE to set
		buffer[size - remaining] = I2C1->DR;
		// step 2-f
		I2C1->CR1 &= ~(1<<10);											// clear the ACK bit
		// step 2-g
		I2C1->CR1 |= (1<<9);												// stop I2C
		
		remaining--;
		
		// read the last byte
		while(!(I2C1->SR1 & (1<<6)));								// wait for RxNE to set
		buffer[size - remaining] = I2C1->DR;				// copy the data into the buffer
		
	}
}
