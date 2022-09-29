
#ifndef __GT911_H_
#define __GT911_H_
#include "main.h"



void GT911_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen);
void GT911_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen);
void GT911_Send_Config(uint8_t mode);//配置初始化GT911
void Software_Reset(uint8_t gt_SR_type);//复位或者不复位gt911
void GT911_Set_Addr(void);
void GT911_INT_IT_SET(void);


#endif