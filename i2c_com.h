/*
 * i2c_com.h
 *
 *  Created on: 25 jul. 2019
 *      Author: marti
 */

#ifndef I2C_COM_H_
#define I2C_COM_H_

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "em_i2c.h"

void i2c_com_init(void);
I2C_TransferReturn_TypeDef i2c_com_write_register(uint8_t i2c_slave_addr, uint8_t reg_addr, uint8_t *write_buf, uint16_t write_buf_len);
I2C_TransferReturn_TypeDef i2c_com_read_register(uint8_t i2c_slave_addr, uint8_t reg_addr, uint8_t *read_buf, uint16_t read_buf_len);

#endif /* I2C_COM_H_ */
