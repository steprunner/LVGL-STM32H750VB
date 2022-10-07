#ifndef __APP_H_
#define __APP_H_

/* 板级支持包 */
#include "bsp.h"

/* LVGL */
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demos.h"

/* 各类LVGL应用 */
#include "app_test.h"

/* app外部函数 */
void app_lvgl_init(void);
void app_lvgl_disp(void);


#endif

