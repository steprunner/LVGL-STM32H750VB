#ifndef __BSP_GT911_H_
#define __BSP_GT911_H_

typedef struct
{
	uint16_t x;
	uint16_t y;
}coord_pos;

void bsp_GT911_init(void);
void GT911_SCAN(coord_pos* pos);


#endif