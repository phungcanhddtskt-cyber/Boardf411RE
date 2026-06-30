/*
 * i2c_dev.c
 *
 *  Created on: 30 thg 4, 2026
 *      Author: PhungCanh
 */
#include "i2c_dev.h"

//                                i2c basic
HAL_StatusTypeDef I2C_WriteReg(I2C_Dev_t *dev,uint16_t memaddr,uint16_t memaddr_size,uint8_t *TxBuf,uint16_t len)
{
	return HAL_I2C_Mem_Write(dev->hi2c,dev->dev_addr,memaddr,memaddr_size,TxBuf,len,dev->time_out);
}
HAL_StatusTypeDef I2C_ReadReg(I2C_Dev_t *dev,uint16_t memaddr,uint16_t memaddre_size, uint8_t *RxBuf,uint16_t len)
{
	return HAL_I2C_Mem_Read(dev->hi2c,dev->dev_addr,memaddr,memaddr_size,RxBuf,len,dev->time_out);
}

//                             i2c 1byte

HAL_StatusTypeDef I2C_WriteByte(I2C_Dev_t *dev,uint16_t memaddr,uint8_t *Txdata)
{
	return I2C_WriteReg(dev,memaddr,I2C_MEMADD_SIZE_8BIT,Txdata,1);
}
HAL_StatusTypeDef I2C_ReadByte(I2C_Dev_t *dev,uint16_t memaddr,uint8_t *Rxdata)
{
	return I2C_ReadReg(dev,memaddr,I2C_MEMADD_SIZE_8BIT,Rxdata,1);

}

//                               register 2byte
uint16_t I2C_ReadU16_BE(I2C_Dev_t *dev,uint16_t memaddr)
{
	uint8_t RxBuf[2];
    I2C_ReadReg(dev,memaddr,I2C_MEMADD_SIZE_8BIT,RxBuf,2);
    return (RxBuf[0]<<8)|RxBuf[1];
}
uint16_t I2C_ReadU16_LE(I2C_Dev_t *dev,uint16_t memaddr)
{
	uint8_t RxBuf[2];
    I2C_ReadReg(dev,memaddr,I2C_MEMADD_SIZE_8BIT,RxBuf,2);
    return (RxBuf[1]<<8)|RxBuf[0];
}


//                              no register *raw
HAL_StatusTypeDef I2C_WriteRaw(I2C_Dev_t *dev, uint8_t *TxBuf,uint16_t len)
{
	return HAL_I2C_Master_Transmit(dev->hi2c,dev->dev_addr,*TxBuf,len,dev->time_out);
}
HAL_StatusTypeDef I2C_ReadRaw(I2C_Dev_t *dev, uint8_t *RxBuf,uint16_t len)
{
	return HAL_I2C_Master_Receive(dev->hi2c,dev->dev_addr,RxBuf,len,dev->time_out);
}
