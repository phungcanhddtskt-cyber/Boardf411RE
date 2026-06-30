/*
 * i2c_dev.h
 *
 *  Created on: 28 thg 4, 2026
 *      Author: PhungCanh
 */

#ifndef INC_I2C_DEV_H_
#define INC_I2C_DEV_H_
#include "stm32f4xx_hal.h"
#include "stdint.h"
//struct i2c
typedef struct
{
	I2C_HandleTypeDef *hi2c; // Bus I2C
	uint16_t dev_addr;       // address device (shift<<1)
	uint16_t time_out;       //timeout
} I2C_Dev_t;

//                i2c 1byte or nbyte
HAL_StatusTypeDef I2C_WriteReg(I2C_Dev_t *dev,uint16_t memaddr,uint16_t memaddr_size,uint8_t *TxBuf,uint16_t len);
HAL_StatusTypeDef I2C_ReadReg(I2C_Dev_t *dev,uint16_t memaddr,uint16_t memaddr_size,uint8_t *RxBuf,uint16 len);


//                i2c 1byte
HAL_StatusTypeDef I2C_WriteByte(I2C_Dev_t *dev,uint16_t memaddr,uint8_t *value);
HAL_StatusTypeDef I2C_ReadByte(I2C_Dev_t *dev, uint16_t memaddr, uint8_t *value);


// i2c raw * regiters are not used * not memaddress
HAL_StatusTypeDef I2C_WriteRaw(I2C_Dev_t *dev, uint8_t *TxBuf, uint16_t len);
HAL_StatusTypeDef I2C_ReadRaw(I2C_Dev_t *dev, uint8_t *RxBuf, uint16_t len);


//                 util api
uint16_t I2C_ReadU16_BE(I2C_Dev_t *dev, uint8_t memaddr);
uint16_t I2C_ReadU16_LE(I2C_Dev_t *dev, uint8_t memaddr);


#endif /* INC_I2C_DEV_H_ */
