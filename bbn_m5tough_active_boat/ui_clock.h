#ifndef UI_CLOCK_H
#define UI_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

  lv_updatable_screen_t clockScreen;

  static lv_obj_t *clock_display;

  lv_meter_indicator_t *indic_sec;
  lv_meter_indicator_t *indic_min;
  lv_meter_indicator_t *indic_hour;

  lv_obj_t *labelDate;
  lv_obj_t *labelTime;

  /**
   * A clock display 
   */
  void lv_clock_display(lv_obj_t *parent) {
    clock_display = lv_meter_create(parent);
    lv_obj_set_size(clock_display, 220, 220);
    lv_obj_center(clock_display);

    labelDate = lv_label_create(parent);
    lv_obj_set_pos(labelDate, 10, 20);
    lv_obj_align(labelDate, LV_ALIGN_TOP_LEFT, 10, 20);

    labelTime = lv_label_create(parent);
    lv_obj_set_pos(labelTime, 260, 20);
    lv_obj_align(labelTime, LV_ALIGN_TOP_LEFT, 260, 20);

    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t *scale = lv_meter_add_scale(clock_display);
    lv_meter_set_scale_ticks(clock_display, scale, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(clock_display, scale, 0, 60, 360, 270);

    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(clock_display);
    lv_meter_set_scale_ticks(clock_display, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));           /*12 ticks*/
    lv_meter_set_scale_major_ticks(clock_display, scale_hour, 1, 2, 20, lv_palette_main(LV_PALETTE_GREY), 10); /*Every tick is major*/
    lv_meter_set_scale_range(clock_display, scale_hour, 1, 12, 330, 300);                                      /*[1..12] values in an almost full circle*/

    indic_sec = lv_meter_add_needle_line(clock_display, scale, 2, lv_palette_main(LV_PALETTE_GREY), -10);
    indic_min = lv_meter_add_needle_line(clock_display, scale, 4, lv_palette_main(LV_PALETTE_GREEN), -20);
    indic_hour = lv_meter_add_needle_line(clock_display, scale, 7, lv_palette_main(LV_PALETTE_RED), -42);
  }

  static void set_clock_value(void *indic, int32_t v) {
    lv_meter_set_indicator_value(clock_display, (lv_meter_indicator_t *)indic, v);
  }

  static void clock_update_cb() {
    M5.Rtc.GetDate(&RTCdate);
    M5.Rtc.GetTime(&RTCtime);
    set_clock_value(indic_hour, RTCtime.Hours);
    set_clock_value(indic_min, RTCtime.Minutes);
    set_clock_value(indic_sec, RTCtime.Seconds);
    lv_label_set_text(labelDate,
                      (String() + RTCdate.Year + "-" + RTCdate.Month + "-" + RTCdate.Date).c_str());
    lv_label_set_text(labelTime,
                      (String() + RTCtime.Hours + ":" + RTCtime.Minutes + ":" + RTCtime.Seconds).c_str());
  }

  void init_clockScreen() {
    clockScreen.screen = lv_obj_create(NULL);  // Creates a Screen object
    lv_clock_display(clockScreen.screen);
    clockScreen.update_cb = clock_update_cb;
  }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
