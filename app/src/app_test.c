#include "app.h"


void app_test(void)
{
  lv_obj_t *btn = lv_btn_create(lv_scr_act());
  lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_size(btn, 120, 47);

  lv_obj_t *label;
  label = lv_label_create(btn);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text(label, "HELLO LVGL");

  static lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_radius(&style_btn, 10);
  lv_style_set_border_color(&style_btn, lv_color_white());
  lv_style_set_border_opa(&style_btn, LV_OPA_30);
  lv_obj_add_style(btn, &style_btn, LV_STATE_DEFAULT);
}


