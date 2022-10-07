#include "app.h"



/* lvgl驱动初始化 */
void app_lvgl_init(void)
{
  bsp_init();
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
}
/* lvgl动画 */
void app_lvgl_disp(void)
{
  /* test */
  app_test();

}

