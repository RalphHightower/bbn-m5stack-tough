#ifndef UI_COMPASS_H
#define UI_COMPASS_H

#ifdef __cplusplus
extern "C" {
#endif

  lv_updatable_screen_t compassScreen;

  static lv_obj_t *compass_display;
  static lv_obj_t *compass_l;

  static lv_obj_t *labelScont;
  static lv_obj_t *labelNcont;
  static lv_obj_t *labelEcont;
  static lv_obj_t *labelWcont;

  static lv_meter_scale_t *scale_compass;
  static lv_meter_scale_t *scale_compass_maj;

  /**
   * A compass display
   */
  void lv_compass_display(lv_obj_t *parent) {
    compass_display = lv_meter_create(parent);

    lv_obj_remove_style(compass_display, NULL, LV_PART_MAIN);
    lv_obj_remove_style(compass_display, NULL, LV_PART_INDICATOR);

    lv_obj_set_size(compass_display, 190, 190);
    lv_obj_center(compass_display);

    scale_compass = lv_meter_add_scale(compass_display);
    lv_meter_set_scale_ticks(compass_display, scale_compass, 73, 1, 12, lv_palette_main(LV_PALETTE_GREY));

    scale_compass_maj = lv_meter_add_scale(compass_display);
    lv_meter_set_scale_ticks(compass_display, scale_compass_maj, 12, 2, 15, lv_palette_main(LV_PALETTE_GREY)); /*12 ticks*/

    labelNcont = lv_obj_create(parent);
    lv_obj_set_size(labelNcont, 50, 50);
    lv_obj_set_style_bg_color(labelNcont, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    lv_obj_align(labelNcont, LV_ALIGN_CENTER, 0, -48);

    lv_obj_t *labelN = lv_label_create(labelNcont);
    lv_label_set_text(labelN, "N");
    lv_obj_align(labelN, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_transform_pivot_x(labelNcont, 50 / 2, 0);
    lv_obj_set_style_transform_pivot_y(labelNcont, 50 / 2 + 48, 0);

    labelScont = lv_obj_create(parent);
    lv_obj_set_size(labelScont, 50, 50);
    lv_obj_set_style_bg_color(labelScont, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
    lv_obj_align(labelScont, LV_ALIGN_CENTER, 0, -48);

    lv_obj_t *labelS = lv_label_create(labelScont);
    lv_label_set_text(labelS, "S");
    lv_obj_align(labelS, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_transform_pivot_x(labelScont, 50 / 2, 0);
    lv_obj_set_style_transform_pivot_y(labelScont, 50 / 2 + 48, 0);

    labelEcont = lv_obj_create(parent);
    lv_obj_set_size(labelEcont, 50, 50);
    lv_obj_align(labelEcont, LV_ALIGN_CENTER, 0, -48);

    lv_obj_t *labelE = lv_label_create(labelEcont);
    lv_label_set_text(labelE, "E");
    lv_obj_align(labelE, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_transform_pivot_x(labelEcont, 50 / 2, 0);
    lv_obj_set_style_transform_pivot_y(labelEcont, 50 / 2 + 48, 0);

    labelWcont = lv_obj_create(parent);
    lv_obj_set_size(labelWcont, 50, 50);
    lv_obj_align(labelWcont, LV_ALIGN_CENTER, 0, -48);

    lv_obj_t *labelW = lv_label_create(labelWcont);
    lv_label_set_text(labelW, "W");
    lv_obj_align(labelW, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_transform_pivot_x(labelWcont, 50 / 2, 0);
    lv_obj_set_style_transform_pivot_y(labelWcont, 50 / 2 + 48, 0);

    lv_obj_t *compass_mark_l = lv_label_create(parent);
    lv_label_set_text(compass_mark_l, LV_SYMBOL_DOWN);
    lv_obj_align(compass_mark_l, LV_ALIGN_CENTER, 0, -100);

    compass_l = lv_label_create(parent);
    lv_label_set_text(compass_l, "--" LV_SYMBOL_DEGREES);
    //lv_obj_set_width(compass_l, width_l);
    //lv_obj_set_style_text_align(compass_l, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(compass_l, LV_ALIGN_CENTER, 0, 0);
#if LV_FONT_MONTSERRAT_20
    lv_obj_set_style_text_font(compass_l, &lv_font_montserrat_20, NULL);
#endif

    lv_meter_set_scale_range(compass_display, scale_compass, 0, 72, 360, 270);
    lv_meter_set_scale_range(compass_display, scale_compass_maj, 1, 12, 330, 300);
    lv_obj_set_style_transform_angle(labelScont, 180 * 10, 0);
    lv_obj_set_style_transform_angle(labelEcont, 90 * 10, 0);
    lv_obj_set_style_transform_angle(labelWcont, 270 * 10, 0);
  }

  static int16_t last_heading = 0;

  static void compass_update_cb() {
    int16_t h_deg = fresh(shipDataModel.navigation.heading_mag.age) ? shipDataModel.navigation.heading_mag.deg : 0;
    if (last_heading != h_deg) {
      int rot = h_deg;
      lv_meter_set_scale_range(compass_display, scale_compass, 0, 72, 360, 270 + rot);
      lv_meter_set_scale_range(compass_display, scale_compass_maj, 1, 12, 330, 300 + rot);
      lv_obj_set_style_transform_angle(labelNcont, rot * 10, 0);
      lv_obj_set_style_transform_angle(labelScont, (180 + rot) * 10, 0);
      lv_obj_set_style_transform_angle(labelEcont, (90 + rot) * 10, 0);
      lv_obj_set_style_transform_angle(labelWcont, (270 + rot) * 10, 0);

      lv_label_set_text(compass_l,
                        (fresh(shipDataModel.navigation.heading_mag.age) ? String(shipDataModel.navigation.heading_mag.deg, 0) + LV_SYMBOL_DEGREES : "--").c_str());
    }
  }

  void init_compassScreen() {
    compassScreen.screen = lv_obj_create(NULL);  // Creates a Screen object
    lv_compass_display(compassScreen.screen);
    compassScreen.update_cb = compass_update_cb;
  }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
