#include "GT911.h"
#include "i2c.h"
#include "usart.h"
#include "lcd_drv.h"
/*I2C句柄*/
#define		GT911_I2C		hi2c1
#define		GT911_DIV_ID	0XBA	//设备地址 //0X28 //0XBA
#define 	GT911_DIV_W		(GT911_DIV_ID | 0)	//写地址
#define 	GT911_DIV_R		(GT911_DIV_ID | 1)	//读地址

#define RST_DOWN()      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET)
#define RST_UP()        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET)
#define WAKE_DOWN()     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET)
#define WAKE_UP()       HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET)
#define INT_DOWN()      HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET)
#define INT_UP()        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET)
#define INT_Read         HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)

//GT911 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT911控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT911配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT911校验和寄存器
#define GT_PID_REG 		0X8140   	//GT911产品ID寄存器
 
#define GT_GSTID_REG 	0X814E   	//GT911当前检测到的触摸情况,第7位是触摸标志位，低4位是触摸点数个数

#define GT_TPD_Sta		0X8150		//触摸点起始数据地址
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址


#define GT_TOUCH_MAX	5			//对于gt911，最多同时获取5个触摸点的数据

typedef enum
{
	X_L = 0,
	X_H = 1,
	Y_L = 2,
	Y_H = 3,
	S_L	= 4,
	S_H = 5
}Data_XYS_P;	//数据X、Y、触摸大小数据偏移量

typedef enum
{
	TOUCH__NO		= 0x00,	//没有触摸
	TOUCH_ING		= 0x80	//被触摸	
}TOUCH_STATE_enum;	//触摸状态


typedef struct
{
	uint16_t	X_Point;	//X坐标
	uint16_t	Y_Point;	//Y坐标
	uint16_t	S_Point;	//触摸点大小
}XY_Coordinate;	//触摸点坐标

typedef struct
{
	uint8_t Touch_State				;	//触摸状态
	uint8_t Touch_Number			;	//触摸数量
	XY_Coordinate Touch_XY[GT_TOUCH_MAX]	;	//触摸的x坐标，对于gt911最多5个点的坐标
}Touch_Struct;	//触摸信息结构体

Touch_Struct UserTouch;

//GT911（原GT9147）配置参数表
//第一个字节为版本号(0X60),必须保证新的版本号大于等于GT911内部
//flash原有版本号,才会更新配置.
const uint8_t GT9147_CFG_TBL[]=
{ 
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF
};  

/*修改INT脚为输出*/
void GT911_INT_OUTP(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void GT911_INT_INP(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void GT911_INT_IT_SET(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

/*设置GT911 I2C 地址*/
void GT911_Set_Addr(void)
{
	GT911_INT_OUTP();//设置INT脚为输出
	WAKE_UP();
	RST_DOWN();//拉低RST和INT
	INT_DOWN();
	HAL_Delay(10);//延时10ms
	RST_UP();//RST拉高
	HAL_Delay(10);//延时10ms
}

void GT911_WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	HAL_I2C_Mem_Write(&GT911_I2C, GT911_DIV_W, _usRegAddr, I2C_MEMADD_SIZE_16BIT, _pRegBuf, _ucLen, 0xff);
}

void GT911_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	HAL_I2C_Mem_Read(&GT911_I2C, GT911_DIV_R, _usRegAddr, I2C_MEMADD_SIZE_16BIT, _pRegBuf, _ucLen, 0xff);
}

/*
	配置gt911，发送gt911配置参数
	参数1：mode（0,参数不保存到flash1,参数保存到flash）
*/
void GT911_Send_Config(uint8_t mode)
{
	uint8_t buf[2];
	buf[0] = 0;
	buf[1] = mode;	//是否写入到GT9147 FLASH?  即是否掉电保存
	for(uint8_t i=0; i<sizeof(GT9147_CFG_TBL); i++)
	{
		buf[0] += GT9147_CFG_TBL[i];//计算校验和
	}
  buf[0]=(~buf[0])+1;
	
	GT911_WriteReg(GT_CFGS_REG, (uint8_t *)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL));//发送寄存器配置
	
	GT911_WriteReg(GT_CHECK_REG, buf, 2);//写入校验和,和配置更新标记
}


/*
	功能：软件复位gt911
	参数1：gt_SR_type（为1时开始软件复位,为0时结束软件复位）
*/
void Software_Reset(uint8_t gt_SR_type)
{
	uint8_t _temp=0;	//中间变量
	
	if(gt_SR_type)
	{
		_temp=2;
		GT911_WriteReg(GT_CTRL_REG, &_temp, 1);
	}
	else
	{
		_temp=0;
		GT911_WriteReg(GT_CTRL_REG, &_temp, 1);
	}
}

void GT911_SCAN(void)
{
	uint16_t x,y;
	/*手动延迟避免多次进入触摸*/
//	static uint8_t _timer=0;
//	_timer++;
//	if(_timer<10) return;
//	_timer=0;
	/*读取触摸数据*/
	uint8_t _temp;	//中间变量
	GT911_ReadReg(GT_GSTID_REG, &_temp, 1);
	UserTouch.Touch_Number = (_temp & 0x0f);	//获取触摸点数
	UserTouch.Touch_State = (_temp & 0x80);	//触摸状态
	for(uint8_t i=0; i<UserTouch.Touch_Number; i++)
	{
		GT911_ReadReg((GT_TPD_Sta + i*8 + X_L), &_temp, 1);	//读出触摸x坐标的低8位
		UserTouch.Touch_XY[i].X_Point  = _temp;
		GT911_ReadReg((GT_TPD_Sta + i*8 + X_H), &_temp, 1);	//读出触摸x坐标的高8位
		UserTouch.Touch_XY[i].X_Point |= (_temp<<8);
		
		GT911_ReadReg((GT_TPD_Sta + i*8 + Y_L), &_temp, 1);	//读出触摸y坐标的低8位
		UserTouch.Touch_XY[i].Y_Point  = _temp;
		GT911_ReadReg((GT_TPD_Sta + i*8 + Y_H), &_temp, 1);	//读出触摸y坐标的高8位
		UserTouch.Touch_XY[i].Y_Point |= (_temp<<8);
		
		GT911_ReadReg((GT_TPD_Sta + i*8 + S_L), &_temp, 1);	//读出触摸大小数据的低8位
		UserTouch.Touch_XY[i].S_Point  = _temp;
		GT911_ReadReg((GT_TPD_Sta + i*8 + S_H), &_temp, 1);	//读出触摸大小数据的高8位
		UserTouch.Touch_XY[i].S_Point |= (_temp<<8);
//		printf("%d,%d,%d\n",UserTouch.Touch_XY[i].X_Point,UserTouch.Touch_XY[i].Y_Point,UserTouch.Touch_XY[i].S_Point);
		x=UserTouch.Touch_XY[i].X_Point;
		y=272-UserTouch.Touch_XY[i].Y_Point;
		FillRect(x,y,4,4,  0xFFFF);
	}

	_temp=0;
	GT911_WriteReg(GT_GSTID_REG, &_temp, 1);	//清除数据标志位
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	UNUSED(GPIO_Pin);
	GT911_SCAN();
	
}



