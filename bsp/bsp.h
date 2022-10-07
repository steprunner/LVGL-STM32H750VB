#ifndef __BSP_H_
#define __BSP_H_
/* HAL */
#include "main.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* 各类C语言库 */
#include <stdio.h>
#include <math.h>
#include <string.h>

/* 各类bsp驱动 */

#include "bsp_GT911.h"


/* 外部函数接口 */
void bsp_init(void);

#endif