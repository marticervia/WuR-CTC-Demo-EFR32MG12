#include "i2c_com.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "hal-config.h"
/*
 * i2c_com.c
 *
 *  Created on: 25 jul. 2019
 *      Author: marti
 *  Loosely based on sillicon i2c example from:
 *  https://github.com/SiliconLabs/peripheral_examples/blob/master/series1/i2c/i2c/src/main_efr.c
 */


void i2c_com_init(void){

	// Use ~100khz SCK master mode
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	CMU_ClockEnable(cmuClock_I2C0, true);
	GPIO_PinModeSet(WuR_I2C_SDA_PORT, WuR_I2C_SDA_PIN, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(WuR_I2C_SCL_PORT, WuR_I2C_SCL_PIN, gpioModeWiredAndPullUpFilter, 1);


	I2C0->ROUTEPEN = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
	I2C0->ROUTELOC0 = (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK)) | I2C_ROUTELOC0_SDALOC_LOC16;
	I2C0->ROUTELOC0 = (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK)) | I2C_ROUTELOC0_SCLLOC_LOC14;

	I2C_Init(I2C0, &i2cInit);

}

static I2C_TransferReturn_TypeDef _i2c_com_master_transfer(uint8_t i2c_slave_addr, uint8_t i2c_mode, uint8_t* buffer_1, uint16_t buffer_1_len,\
		uint8_t* buffer_2, uint16_t buffer_2_len){

	// Transfer structure
	I2C_TransferSeq_TypeDef i2cTransfer;
	I2C_TransferReturn_TypeDef result;

	// Initializing I2C transfer
	i2cTransfer.addr          = i2c_slave_addr;
	i2cTransfer.flags         = i2c_mode;
	i2cTransfer.buf[0].data   = buffer_1;
	i2cTransfer.buf[0].len    = buffer_1_len;
	i2cTransfer.buf[1].data   = buffer_2;
	i2cTransfer.buf[1].len    = buffer_2_len;
	result = I2C_TransferInit(I2C0, &i2cTransfer);

	// Sending data on a blocking manner
	while (result == i2cTransferInProgress)
	{
	result = I2C_Transfer(I2C0);
	}

	return result;
}

I2C_TransferReturn_TypeDef i2c_com_write_register(uint8_t i2c_slave_addr, uint8_t reg_addr, uint8_t write_buf, uint16_t write_buf_len){

}

I2C_TransferReturn_TypeDef i2c_com_read_register(uint8_t i2c_slave_addr, uint8_t reg_addr, uint8_t write_buf, uint16_t write_buf_len){

}




